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

using namespace cy;

bool isLeftDraging, isRightDraging;
int prevX, prevY;
float rotationX, rotationY, rotationZ, viewScale, transZ;
float p_rotationX, p_rotationY, p_rotationZ, p_viewScale, p_transZ;
int p_prevX, p_prevY;
ObjDrawer* objDrawer;
ObjDrawer* planeDrawer;

GLuint frameBuffer = 0;
GLint origFB = 0;

GLuint rendTexture;

void displayFunc()
{
	glClearColor(.3, .3, .3, 1);
	glViewport(0, 0, 1024, 1024);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	objDrawer->drawTri();

	glViewport(0, 0, 1920, 1080);
	glClearColor(0, 0, 0, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, origFB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glBindTexture(GL_TEXTURE_2D, rendTexture);
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
			if (glutGetModifiers() == GLUT_ACTIVE_ALT) {
				isLeftDraging = true;
				p_prevX = x;
				p_prevY = y;
			}
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
			if (glutGetModifiers() == GLUT_ACTIVE_ALT) {
				isRightDraging = true;
				p_prevX = x;
				p_prevY = y;
			}
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

	int p_deltaX = x - p_prevX;
	int p_deltaY = y - p_prevY;

	if (isLeftDraging) {
		if (glutGetModifiers() == GLUT_ACTIVE_ALT) {
			p_rotationX += (float)p_deltaY / 500;
			p_rotationZ += (float)p_deltaX / 500;
			planeDrawer->setMV(p_rotationX, p_rotationY, p_rotationZ, p_viewScale, p_transZ);
		}
		else {
			rotationX += (float)deltaY / 500;
			rotationZ += (float)deltaX / 500;
			objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
		}
	}
	else if (isRightDraging) {
		if (glutGetModifiers() == GLUT_ACTIVE_ALT) {
			p_transZ += (float)p_deltaY / 500;
			planeDrawer->setMV(p_rotationX, p_rotationY, p_rotationZ, p_viewScale, p_transZ);
		}
		else {
			transZ += (float)deltaY / 500;
			objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
		}
	}

	prevX = x;
	prevY = y;

	p_prevX = x;
	p_prevY = y;

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

float* getMVP(float rotateX, float rotateY, float rotateZ, float scale, float transformZ)
{
	float rx[] = { 1, 0, 0, 0, 0, cos(rotateX), sin(rotateX), 0, 0, -sin(rotateX), cos(rotateX), 0, 0, 0, 0, 1 };
	Matrix4<float> m_rotateX(rx);

	float ry[] = { cos(rotateY), 0, -sin(rotateY), 0, 0, 1, 0, 0, sin(rotateY), 0, cos(rotateY), 0, 0, 0, 0, 1 };
	Matrix4<float> m_rotateY(ry);

	float rz[] = { cos(rotateZ), sin(rotateZ), 0, 0, -sin(rotateZ), cos(rotateZ), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	Matrix4<float> m_rotateZ(rz);

	float s[] = { scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, 1 };
	Matrix4<float> m_scale(s);

	float t[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, transformZ, 1 };
	Matrix4<float> m_trans(t);

	float t2[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -.3, 1 };
	Matrix4<float> m_trans2(t2);

	Matrix4f mvp = m_trans * m_rotateX * m_rotateY * m_rotateZ * m_trans2 * m_scale;

	float sending[16];
	Matrix4f m_pres = Matrix4f::Perspective(1.6f, 1, 0.1f, 100.f);
	(m_pres * mvp).Get(sending);

	return sending;
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

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Project 6");
	setupFuncs();
	

	// GLEW Init
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glutInitContextVersion(4, 5);
	glEnable(GL_DEPTH_TEST);

	// Load Mesh
	glActiveTexture(GL_TEXTURE0);
	objDrawer = new ObjDrawer(argv[1], true);

	// Set up VS FS
	objDrawer->setVS("SimpleVS.glsl");
	objDrawer->setFS("SimpleFS.glsl");

	// Set mvp
	objDrawer->setAttrib("pos");
	objDrawer->setCameraSize(1024, 1024);
	objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);

	glActiveTexture(GL_TEXTURE1);
	// Plane setup
	planeDrawer = new ObjDrawer("res/plane.obj", false);
	planeDrawer->setVS("SceneVS.glsl");
	planeDrawer->setFS("SceneFS.glsl");

	planeDrawer->setAttrib("pos");
	planeDrawer->setCameraSize(width, height);
	planeDrawer->setMV(p_rotationX, p_rotationY, p_rotationZ, p_viewScale, p_transZ);

	// get origFrameBuffer
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &origFB);

	// Gen Frame Buffer & Bind
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Gen Texture
	glActiveTexture(GL_TEXTURE1);
	glGenTextures(1, &rendTexture);
	glBindTexture(GL_TEXTURE_2D, rendTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1024, 1024, 0, GL_RGB, GL_UNSIGNED_BYTE, 0);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

	// Depth Buffer
	GLuint depthBuffer;
	glGenRenderbuffers(1, &depthBuffer);
	glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, 1024, 1024);

	// configure Frame Buffer
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
	glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, rendTexture, 0);
	GLenum drawBuffer[1] = { GL_COLOR_ATTACHMENT0 };
	glDrawBuffers(1, drawBuffer);
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Frame Buffer not complete" << std::endl;

	planeDrawer->setTexUnit(1);

	displayFunc();

	glutMainLoop();

	return 0;
}



//Screen constants