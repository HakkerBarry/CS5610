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
ObjDrawer* objDrawer;
TriMesh plane;
GLSLProgram planeP;

GLuint frameBuffer = 0;
GLint origFB = 0;

void displayFunc()
{
	/*glClearColor(.5, .5, .5, 1);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	objDrawer->drawTri();
	glutSwapBuffers();*/

	glClearColor(0, 0, 0, 0);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, origFB);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	planeP.Bind();
	glDrawArrays(GL_TRIANGLES, 0, 50000);
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
		rotationX += (float)deltaY / 500;
		rotationZ += (float)deltaX / 500;
		objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
	}
	else if (isRightDraging) {
		transZ += (float)deltaY / 500;
		objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
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
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	viewScale = .05;
	transZ = -2;
	int width = 1920;
	int height = 1080;
	//GLUT Init
	glutInit(&argc, argv);

	glutInitWindowSize(width, height);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Project 5");
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
	objDrawer->setCameraSize(width, height);
	objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);

	// get origFrameBuffer
	glGetIntegerv(GL_FRAMEBUFFER_BINDING, &origFB);

	// Gen Frame Buffer & Bind
	glGenFramebuffers(1, &frameBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	// Gen Texture
	GLuint rendTexture;
	glGenTextures(1, &rendTexture);
	glActiveTexture(GL_TEXTURE1);
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

	// Init plane shader
	GLSLShader p_vs, p_fs;
	p_vs.CompileFile("SceneVS.glsl", GL_VERTEX_SHADER);
	p_fs.CompileFile("SceneFS.glsl", GL_FRAGMENT_SHADER);
	planeP.AttachShader(p_vs);
	planeP.AttachShader(p_fs);
	planeP.Link();

	// Load plane
	std::vector<Vec3<float>> planeV;
	std::vector<Vec2<float>> p_texC;
	int f_num = plane.NF();
	plane.LoadFromFileObj("res/plane.obj", true);
	
	for (int i = 0; i < f_num; ++i) {
		planeV.push_back(plane.V(plane.F(i).v[0]));
		planeV.push_back(plane.V(plane.F(i).v[1]));
		planeV.push_back(plane.V(plane.F(i).v[2]));
		p_texC.push_back(Vec2<float>(plane.GetTexCoord(i, Vec3f(1, 0, 0))));
		p_texC.push_back(Vec2<float>(plane.GetTexCoord(i, Vec3f(0, 1, 0))));
		p_texC.push_back(Vec2<float>(plane.GetTexCoord(i, Vec3f(0, 0, 1))));
	}

	// plane vertex
	GLuint PlaneV;
	glGenBuffers(1, &PlaneV);
	glBindBuffer(GL_ARRAY_BUFFER, PlaneV);
	glBufferData(GL_ARRAY_BUFFER, planeV.size() * sizeof(Vec3<float>), planeV.data(), GL_STATIC_DRAW);

	// plane tex Coord
	GLuint PlaneTexC;
	glGenBuffers(1, &PlaneTexC);
	glBindBuffer(GL_ARRAY_BUFFER, PlaneTexC);
	glBufferData(GL_ARRAY_BUFFER, p_texC.size() * sizeof(Vec2<float>), p_texC.data(), GL_STATIC_DRAW);

	//buffer plane data
	glUseProgram(planeP.GetID());
	GLuint p_v_loc = glGetAttribLocation(planeP.GetID(), "pos");
	glBindBuffer(GL_ARRAY_BUFFER, PlaneV);
	glEnableVertexAttribArray(p_v_loc);
	glVertexAttribPointer(p_v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	GLuint p_texC_loc = glGetAttribLocation(planeP.GetID(), "texc");
	glBindBuffer(GL_ARRAY_BUFFER, PlaneTexC);
	glEnableVertexAttribArray(p_texC_loc);
	glVertexAttribPointer(p_texC_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	GLuint p_mvp_loc = glGetUniformLocation(planeP.GetID(), "mvp");
	glUniformMatrix4fv(p_mvp_loc, 1, false, getMVP(rotationX, rotationY, rotationZ, viewScale, transZ));
	GLuint p_tex_loc = glGetUniformLocation(planeP.GetID(), "tex");
	glUniform1i(p_tex_loc, 1);

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
	displayFunc();

	glutMainLoop();

	return 0;
}



//Screen constants