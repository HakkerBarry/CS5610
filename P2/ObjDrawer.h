#pragma once
#include "cyTriMesh.h"
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
	int v_num;
	
public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setVS(char const* filename);
	void setFS(char const* filename);
	void drawV();
	void setAttrib(char const* v);
};

