/*
Zixuan Zhang A1 for CS5610 at the UofU
*/

#include <glew.h>
#include<GL/freeglut.h>
#include<GL/gl.h>
#include <GL/glu.h>
#include <stdio.h>
#include "cyTriMesh.h"
#include "cyVector.h"
#include "cyGL.h"
#include <vector>

using namespace cy;

bool isDraging;
int prevX, prevY;

void displayFunc()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int x, int y)
{
	switch (key) {
	case 27:
		glutLeaveMainLoop();
		break;
	}
}

void mouseFunc(int button, int state, int x, int y){
	if (button == GLUT_LEFT_BUTTON)
	{
		if (state == GLUT_DOWN) isDraging = true;
		if (state == GLUT_UP) isDraging = false;
	}
}

void motionFunc(int x, int y)
{
	if (isDraging) {

	}
}

void specialFunc(int key, int x, int y) {

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

void refresh() 
{

}

int main(int argc, char** argv)
{
	//GLUT Init
	glutInit(&argc, argv);

	glutInitWindowSize(1920, 1080);

	glutInitWindowPosition(100, 100);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("CS 5610 Project 2");
	setupFuncs();

	// GLEW Init
	GLenum res = glewInit();
	if (res != GLEW_OK)
	{
		fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
		return 1;
	}

	// Load Mesh
	TriMesh obj;
	obj.LoadFromFileObj("res/teapot.obj", false, &std::cout);
	
	// Set up VS FS
	GLSLShader vs, fs;
	vs.CompileFile("SimpleVS.glsl", GL_VERTEX_SHADER);
	fs.CompileFile("SimpleFS.glsl", GL_FRAGMENT_SHADER);

	// Link as program
	GLSLProgram prog;
	prog.CreateProgram();
	prog.AttachShader(vs);
	prog.AttachShader(fs);
	prog.Link();

	// Set mvp
	prog.Bind();

	// set Vertex
	int numVertex = obj.NV();
	Vec3<float> vertices[5000];
	for (int i = 1; i < numVertex; ++i) {
		vertices[i] = obj.V(i);
	}

	// Set mvp
	prog.Bind();
	GLint mvp = glGetUniformLocation(prog.GetID(), "mvp");
	float temp[] = { .05, 0, 0, 0, 0, .05, 0, 0, 0, 0, .05, 0, 0, 0, 0, 1 };
	glUniformMatrix4fv(mvp, 1, false, temp);
	

	// V buffer object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, numVertex);

	glutSwapBuffers();

	glutMainLoop();

	return 0;
}



//Screen constants