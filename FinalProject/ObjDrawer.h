#pragma once
#ifndef OBJ_DRAWER
#define OBJ_DRAWER


#include <GL/gl.h>
#include <glew.h>
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyGL.h"
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>

using namespace cy;

class ObjDrawer
{
private:
	TriMesh obj;
	//GLSLShader vs, fs;
	//GLSLProgram prog;
	glm::vec3 position;
	glm::vec3 scale;
	GLuint p_id;
	GLuint VBO;
	GLint v_loc;
	Matrix4<float> mvp;
	int v_num;
	bool isPerspect;

	void sendMVP();

public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setProg(GLuint id);
	void drawV();
	void setAttrib(char const* v);
	void setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ);
	void setPerspect(bool isPerspect);
	void draw(glm::mat4 v, glm::mat4 p);
	void setPosition(glm::vec3 pos);
	void setScale(glm::vec3 scale);
};

#endif
