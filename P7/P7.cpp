/*
Zixuan Zhang A5 for CS5610 at the UofU
*/

#include <glew.h>
#include<GL/freeglut.h>
#include<GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "cyTriMesh.h"
#include "cyVector.h"
#include "cyGL.h"
#include "ObjDrawer.h"
#include <vector>
#include "Camera.h"

using namespace cy;

bool isLeftDraging, isRightDraging;
int prevX, prevY;
float rotationX, rotationY, rotationZ, viewScale, transZ;
float p_rotationX, p_rotationY, p_rotationZ, p_viewScale, p_transZ;
int p_prevX, p_prevY;
ObjDrawer* objDrawer;
ObjDrawer* planeDrawer;

GLSLProgram p_prog;
GLSLProgram t_prog;
GLSLProgram depth_prog;

GLuint frameBuffer = 0;
GLint origFB = 0;

GLuint rendTexture;

void displayFunc()
{
	// Draw Depth Buffer
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	//glBindFramebuffer(GL_DRAW_FRAMEBUFFER, origFB);
	glViewport(0, 0, 1024, 1024);
	glClear(GL_DEPTH_BUFFER_BIT);
	//objDrawer->setProg(depth_prog.GetID());
	//planeDrawer->setProg(depth_prog.GetID());
	objDrawer->setCameraSize(1024, 1024);
	planeDrawer->setCameraSize(1024, 1024);
	objDrawer->setAttrib();
	planeDrawer->setAttribPlane();
	Matrix4<float> MLP = objDrawer->setMV(0.957999647, -0.318000615, 0, 0.05, 1.95399976);
	planeDrawer->setMV(0.957999647, -0.318000615, 0, 0.05, 1.95399976);
	objDrawer->drawTri();
	planeDrawer->drawTri();

	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, origFB);
	glViewport(0, 0, 1920, 1080);
	glClearColor(0, 1, 0, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	objDrawer->setProg(t_prog.GetID());
	planeDrawer->setProg(p_prog.GetID());
	objDrawer->setCameraSize(1920, 1080);
	planeDrawer->setCameraSize(1920, 1080);
	objDrawer->setAttrib();
	planeDrawer->setAttribPlane();

	objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
	planeDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);

	float mlp[16];
	MLP.Get(mlp);
	objDrawer->setMLP(mlp);
	planeDrawer->setMLP(mlp);
	objDrawer->drawTri();
	planeDrawer->drawTri();


	glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		break;
	}
}

void mouseFunc(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN) {
			isLeftDraging = true;
			prevX = x;
			prevY = y;
		}
		if (state == GLUT_UP) {
			isLeftDraging = false;
		}
	}

	if (button == GLUT_RIGHT_BUTTON) {
		if (state == GLUT_DOWN) {
			isRightDraging = true;
			prevX = x;
			prevY = y;
		}
		if (state == GLUT_UP) {
			isRightDraging = false;
		}
	}
}

void motionFunc(int x, int y)
{
	int deltaX = x - prevX;
	int deltaY = y - prevY;

	if (isLeftDraging) {
		rotationY -= (float)deltaX / 500;
		rotationX -= (float)deltaY / 500;
		objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
		planeDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
	}
	else if (isRightDraging) {
		transZ += (float)deltaY / 500;
		objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
		planeDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
	}

	prevX = x;
	prevY = y;

	displayFunc();
}

void specialFunc(int key, int x, int y) {
	if (key == GLUT_KEY_F6) {
		objDrawer->resetGLProg();
	}
}

void idleFunc()
{

}

void setupFuncs() {
	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);
	glutIdleFunc(idleFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
}


int main(int argc, char** argv)
{
	// teapot
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	viewScale = .05;
	transZ = -2;

	// plane
	p_rotationX = -90;
	p_rotationY = 0;
	p_rotationZ = 0;
	p_viewScale = .05;
	p_transZ = -2;

	int width = 1920;
	int height = 1080;
	//GLUT Init
	glutInit(&argc, argv);
	glutInitContextVersion(4, 5);
	glutInitContextProfile(GLUT_CORE_PROFILE);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Project 7");
	glViewport(0, 0, 1920, 1080);
	setupFuncs();

	// GLEW Init
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// GL enable
	glEnable(GL_DEPTH_TEST);
	glActiveTexture(GL_TEXTURE0);

	// Gen depth buffer
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &origFB);
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// depth map
	GLuint depthMap;
	glGenTextures(1, &depthMap);
	glBindTexture(GL_TEXTURE_2D, depthMap);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	// config frame buffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame Buffer not complete" << std::endl;

	// Teapot shader
	t_prog.CreateProgram();
	GLSLShader t_vs, t_fs;
	t_vs.CompileFile("regularVS.glsl", GL_VERTEX_SHADER);
	t_fs.CompileFile("regularFS.glsl", GL_FRAGMENT_SHADER);
	t_prog.AttachShader(t_vs);
	t_prog.AttachShader(t_fs);
	t_prog.Link();

	// Plane shader
	p_prog.CreateProgram();
	GLSLShader p_vs, p_fs;
	p_vs.CompileFile("planeVS.glsl", GL_VERTEX_SHADER);
	p_fs.CompileFile("planeFS.glsl", GL_FRAGMENT_SHADER);
	p_prog.AttachShader(p_vs);
	p_prog.AttachShader(p_fs);
	p_prog.Link();

	//Teapot
	objDrawer = new ObjDrawer(argv[1], false);
	objDrawer->setProg(t_prog.GetID());
	objDrawer->setTexUnit(0);
	objDrawer->setAttrib();

	//plane
	planeDrawer = new ObjDrawer("res/plane.obj", false);
	planeDrawer->setProg(p_prog.GetID());
	planeDrawer->setTexUnit(0);
	planeDrawer->setAttribPlane();

	//depth map shader
	depth_prog.CreateProgram();
	GLSLShader d_vs, d_fs;
	d_vs.CompileFile("depthVS.glsl", GL_VERTEX_SHADER);
	d_fs.CompileFile("depthFS.glsl", GL_FRAGMENT_SHADER);
	depth_prog.AttachShader(d_vs);
	depth_prog.AttachShader(d_fs);
	depth_prog.Link();

	glutMainLoop();
}



//Screen constants