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
	GLuint prog_id;

	// BUFFER
	GLuint VAO;
	GLuint VBO;
	GLuint tris;
	GLuint NB;
	GLuint TCB;
	
	// attrib location
	GLint v_loc;
	GLint n_loc;
	GLint tc_loc;

	// texture
	GLuint tex;

	Matrix4<float> mvp;
	float camerWidthScale;

public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setProg(GLuint prog);
	void drawV();
	void drawTri();
	void setAttrib();
	void setAttribPlane();
	void setCameraSize(int width, int height);
	Matrix4<float> setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ);
	void setMLP(float* MLP);
	void resetGLProg();
	void setTexUnit(GLint u);
};

#endif
