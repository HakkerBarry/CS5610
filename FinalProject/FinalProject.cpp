/*
Zixuan Zhang A2 for CS5610 at the UofU
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

#include <glm/glm.hpp>

using namespace cy;

bool isLeftDraging, isRightDraging;
int prevX, prevY;
float rotationX, rotationY, rotationZ, viewScale, transZ;
ObjDrawer* objDrawer;

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	objDrawer->drawV();
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
	}
}

void idleFunc()
{

}

//void timerFunc(int t)
//{
//	glClearColor(0, color, color, 1);
//	if (colorFlag) color -= 0.03;
//	else color += 0.03;
//	if (color > 1) colorFlag = true;
//	else if (color < 0) colorFlag = false;
//	glutPostRedisplay();
//	glutTimerFunc(50, timerFunc, 0);
//}

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
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	viewScale = .05;
	transZ = -2;

	//GLUT Init
	glutInit(&argc, argv);

	glutInitWindowSize(1080, 1080);
	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Project 2");
	setupFuncs();
	glClearColor(0, 0, 0, 0);

	// GLEW Init
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);

	// Load Mesh
	objDrawer = new ObjDrawer("res/teapot.obj", false);

	// Set up VS FS
	GLSLProgram simpleP;
	GLSLShader sim_vs, sim_fs;
	sim_vs.CompileFile("shaders/SimpleVS.glsl", GL_VERTEX_SHADER);
	sim_fs.CompileFile("shaders/SimpleFS.glsl", GL_FRAGMENT_SHADER);
	simpleP.AttachShader(sim_vs);
	simpleP.AttachShader(sim_fs);
	simpleP.Link();
	objDrawer->setProg(simpleP.GetID());

	// Set mvp
	objDrawer->setAttrib("pos");
	objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);

	objDrawer->drawV();

	glutSwapBuffers();

	glutMainLoop();

	return 0;
}



//Screen constants