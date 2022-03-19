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
#include "EnvDrawer.h"

using namespace cy;

bool isLeftDraging, isRightDraging;
int prevX, prevY;
float rotationX, rotationY, rotationZ, viewScale, transZ;
float p_rotationX, p_rotationY, p_rotationZ, p_viewScale, p_transZ;
int p_prevX, p_prevY;
ObjDrawer* objDrawer;
EnvDrawer* envDrawer;

GLuint frameBuffer = 0;
GLint origFB = 0;

GLuint rendTexture;

void displayFunc()
{
	glClearColor(0, 0, 1, 1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	envDrawer->drawEnv();
	objDrawer->drawTri();
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
		envDrawer->setMVP(rotationX, rotationY, rotationZ);
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
	glViewport(0, 0, 1920, 1080);
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
	glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);

	// Load Env
	envDrawer = new EnvDrawer("./res/cube.obj", "./EnvVS.glsl", "./EnvFS.glsl");
	envDrawer->setMVP(0, 0, 0);
	envDrawer->drawEnv();

	objDrawer = new ObjDrawer(argv[1], false);
	objDrawer->setShader("./EnvObjVS.glsl", "./EnvObjFS.glsl");
	objDrawer->setAttrib(envDrawer->getEnvTex());
	objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);


	//displayFunc();

	glutMainLoop();
}



//Screen constants