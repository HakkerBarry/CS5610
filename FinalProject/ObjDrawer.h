#pragma once
#ifndef OBJ_DRAWER
#define OBJ_DRAWER


#include <GL/gl.h>
#include <glew.h>
#include "cyTriMesh.h"
#include "cyMatrix.h"
#include "cyGL.h"
#include "glm/glm.hpp"


using namespace cy;

class ObjDrawer
{
private:
	TriMesh obj;
	GLuint p_id;
	GLuint VBO;
	GLint v_loc;
	glm::mat4 m;
	int v_num;
	bool isPerspect;
	glm::vec3 w_position;
	
	void sendMVP();
	
public:
	ObjDrawer(char const* filename, bool loadMtl);
	void setProg(GLuint);
	void drawV();
	void setAttrib(char const* v);
	void setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ);
	void setPerspect(bool isPerspect);
};

#endif
