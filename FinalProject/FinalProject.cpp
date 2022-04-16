/*
Zixuan Zhang A2 for CS5610 at the UofU
*/

#include <glew.h>
#include <GL/freeglut.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "cyTriMesh.h"
#include "cyGL.h"
#include "ObjDrawer.h"
#include "Camera.h"

using namespace cy;

bool isLeftDraging, isRightDraging;
bool flightMode;
int prevX, prevY;
float rotationX, rotationY, rotationZ, viewScale, transZ;
ObjDrawer* objDrawer;
Camera* camera;

int scr_w, scr_h;

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);
	objDrawer->draw(camera->getView(), camera->getProj());
	glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	if(key == 27)
		glutLeaveMainLoop();
	if (key == 'w' || key == 'W') {
		camera->moveFront(0.02f);
		displayFunc();
	}
	if (key == 's' || key == 'S') {
		camera->moveFront(-0.02f);
		displayFunc();
	}
	if (key == 'a' || key == 'A') {
		camera->moveRight(-0.02f);
		displayFunc();
	}
	if (key == 'd' || key == 'D') {
		camera->moveRight(0.02f);
		displayFunc();
	}
	if (key == 'e' || key == 'E') {
		camera->moveWorldUp(0.02f);
		displayFunc();
	}
	if (key == 'q' || key == 'Q') {
		camera->moveWorldUp(-0.02f);
		displayFunc();
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
	
	if(flightMode){
		camera->rotatePitch(-(float)deltaY / 5);
		camera->rotateYaw((float)deltaX / 5);
	}
	

	if (isLeftDraging) {
		rotationX += (float)deltaY / 500;
		rotationZ += (float)deltaX / 500;
		//objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
		
	}
	else if (isRightDraging) {
		transZ += (float)deltaY / 500;
		//objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);
	}

	prevX = x;
	prevY = y;
	displayFunc();
}

void specialFunc(int key, int x, int y) {
	if (key == GLUT_KEY_F1) {
		flightMode = !flightMode;
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
	//glutMotionFunc(motionFunc);
	glutPassiveMotionFunc(motionFunc);
}

int main(int argc, char** argv)
{
	rotationX = 0;
	rotationY = 0;
	rotationZ = 0;
	viewScale = .05;
	transZ = -2;

	flightMode = false;

	//GLUT Init
	glutInit(&argc, argv);

	scr_w = 1920;
	scr_h = 1080;

	glutInitWindowSize(scr_w, scr_h);
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

	// Load Mesh
	objDrawer = new ObjDrawer("res/teapot.obj", false);

	// setup program
	GLSLProgram simple_prog;
	GLSLShader sim_vs, sim_fs;
	simple_prog.CreateProgram();
	sim_vs.CompileFile("shaders/SimpleVS.glsl", GL_VERTEX_SHADER);
	sim_fs.CompileFile("shaders/SimpleFS.glsl", GL_FRAGMENT_SHADER);
	simple_prog.AttachShader(sim_vs);
	simple_prog.AttachShader(sim_fs);
	simple_prog.Link();

	objDrawer->setProg(simple_prog.GetID());

	// Setup Camera
	camera = new Camera(glm::vec3(0, 0, 0), glm::vec3(1, 0, 0), 45.f, scr_w, scr_h);

	// Set mvp
	objDrawer->setAttrib("pos");
	objDrawer->setPosition(glm::vec3(0, 0, 0));
	objDrawer->setScale(glm::vec3(.05f, .05f, .05f));
	//objDrawer->setMV(rotationX, rotationY, rotationZ, viewScale, transZ);

	//objDrawer->drawV();
	objDrawer->draw(camera->getView(), camera->getProj());

	glutSwapBuffers();

	glutMainLoop();

	return 0;
}



//Screen constants