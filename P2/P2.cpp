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

float color;
bool colorFlag = false;



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
	prog.AttachShader(vs);
	prog.AttachShader(fs);
	prog.Link();

	// Set mvp
	GLint mvp = glGetUniformLocation(prog.GetID(), "mvp");
	prog.Bind();
	GLfloat temp[] = {.05, 0, 0, 0, 0, .05, 0, 0, 0, 0, .05, 0, 0, 0, 0, 1};
	glUniformMatrix4fv(mvp, 1, false, temp);

	// set Vertex
	int numVertex = obj.NV();
	std::vector<Vec3<float>> vertices_buffer;
	Vec3<float> vertices[1000];
	for (int i = 0; i < numVertex; ++i) vertices[i] = obj.V(i);
	

	// V buffer object
	GLuint VBO;
	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_buffer), &vertices_buffer, GL_STATIC_DRAW);

	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, vertices_buffer.size());

	glutSwapBuffers();

	glutMainLoop();

	return 0;
}



//Screen constants