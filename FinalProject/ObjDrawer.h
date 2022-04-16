#pragma once
#ifndef OBJ_DRAWER
#define OBJ_DRAWER


#include <GL/gl.h>
#include <glew.h>
#include "cyTriMesh.h"
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
	//rotation in XYZ order
	glm::vec3 rotation;
	GLuint p_id;
	GLuint VBO, NB, TCB, Tan, Bitan;
	GLint v_loc, f_loc, n_loc, tc_loc, tan_loc, bitan_loc;
	int v_num;
	bool isPerspect;

public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setProg(GLuint id);
	void drawV();
	void setAttrib();
	void setPerspect(bool isPerspect);
	void draw(glm::mat4 v, glm::mat4 p);
	void setPosition(glm::vec3 pos);
	void setScale(glm::vec3 scale);
	void setRotation(glm::vec3 rotate);
};

#endif
