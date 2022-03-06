#pragma once
#ifndef ENV_DRAWER
#define ENV_DRAWER

#include <GL/gl.h>
#include <glew.h>
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyGL.h"

using namespace cy;

class EnvDrawer
{
	TriMesh obj;
	GLSLShader vs, fs;
	GLSLProgram prog;

	//vertex
	GLuint VAO;
	GLuint VBO;
	GLuint TCB;

	GLuint texID[6];

public:
	EnvDrawer(char const* filename);
	void drawEnv();
};

#endif
