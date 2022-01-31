/*
Zixuan Zhang A1 for CS5610 at the UofU
*/

#include<GL/freeglut.h>
#include<GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>

float color;
bool colorFlag = false;

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);

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

void specialFunc(int key, int x, int y) {

}

void idleFunc()
{

}

void timerFunc(int t)
{
	glClearColor(0, color, color, 1);
	if (colorFlag) color -= 0.03;
	else color += 0.03;
	if (color > 1) colorFlag = true;
	else if (color < 0) colorFlag = false;
	glutPostRedisplay();
	glutTimerFunc(50, timerFunc, 0);
}

void setupFuncs() {
	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyboardFunc);
	glutSpecialFunc(specialFunc);
	glutIdleFunc(idleFunc);
	glutTimerFunc(50, timerFunc, 0);
}

int main(int argc, char** argv)
{
	//GLUT Init
	glutInit(&argc, argv);

	glutInitWindowSize(1920, 1080);

	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Project 1");

	setupFuncs();

	// OpenGL Init
	color = 0;
	glClearColor(0, 0, 0, 1);

	glutMainLoop();

	return 0;
}



//Screen constants