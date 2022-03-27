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
	GLuint VAO;
	GLuint VBO;
	GLuint tris;
	GLuint NB;
	// attrib location
	GLint v_loc;
	GLint n_loc;

	// texture
	GLuint tex;

	Matrix4<float> mvp;
	float camerWidthScale;

public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setShader(char const* vs_path, char const* fs_path);
	void drawV();
	void drawTri();
	void setAttrib();
	void setAttribPlane();
	void setCameraSize(int width, int height);
	void setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ);
	void setMVP(Matrix4<float> m, Matrix4<float>v, Matrix4<float>p);
	void resetGLProg();
	void setTexUnit(GLint u);
};

#endif
