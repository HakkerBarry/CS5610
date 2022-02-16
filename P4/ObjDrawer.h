#pragma once
#ifndef OBJ_DRAWER
#define OBJ_DRAWER


#include <GL/gl.h>
#include <glew.h>
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyGL.h"

using namespace cy;

class ObjDrawer
{
private:
	TriMesh obj;
	GLSLShader vs, fs;
	GLSLProgram prog;
	// BUFFER
	GLuint VBO;
	GLuint tris;
	GLuint NB;
	// attrib location
	GLint v_loc;
	GLint f_loc;
	GLint n_loc;

	Matrix4<float> mvp;
	bool isPerspect;
	float camerWidthScale;
	
	void sendMVP();
	
public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setVS(char const* filename);
	void setFS(char const* filename);
	void drawV();
	void drawTri();
	void setAttrib(char const* v, char const* n);
	void setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ);
	void setPerspect(bool isPerspect);
	void resetGLProg();
	void setCameraSize(int width, int height);
};

#endif
