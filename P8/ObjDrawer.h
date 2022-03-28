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

	GLuint progID;

	// BUFFER
	GLuint VAO;
	GLuint VBO;
	GLuint tris;
	GLuint NB;
	GLuint TCB;
	GLuint Tan;
	GLuint Bitan;
	
	// attrib location
	GLint v_loc;
	GLint f_loc;
	GLint n_loc;
	GLint tc_loc;
	GLint tan_loc;
	GLint bitan_loc;

	// texture
	GLuint tex;

	Matrix4<float> mvp;
	bool isPerspect;
	float camerWidthScale;

public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setProg(GLuint prog_id);
	void drawV();
	void drawTri();
	void setAttrib();
	void setNormalTex(char const* nor_path);
	void setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ);
	void setPerspect(bool isPerspect);
	void resetGLProg();
	void setTexUnit(GLint u);
};

#endif
