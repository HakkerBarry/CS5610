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
	GLuint VBO;
	GLint v_loc;
	Matrix4<float> mvp;
	int v_num;
	bool isPerspect;
	
	void sendMVP();
	
public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setVS(char const* filename);
	void setFS(char const* filename);
	void drawV();
	void setAttrib(char const* v);
	void setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ);
	void setPerspect(bool isPerspect);
	void resetGLProg();
};

#endif
