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

	//Buffer Objects
	GLuint VAO;
	GLuint VBO;

	// Attrib location
	GLint v_loc;
	GLint mvp_loc;

	// Texture
	GLuint texID;

public:
	EnvDrawer(char const* filename, char const* vs, char const* fs);
	void setMVP(float rotateX, float rotateY, float rotateZ);
	void drawEnv();
	GLuint getEnvTex();
};

#endif
