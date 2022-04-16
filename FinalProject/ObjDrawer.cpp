#include <glew.h>
#include "ObjDrawer.h"

ObjDrawer::ObjDrawer(char const* filename, bool loadMtl) :
	v_loc(-1), isPerspect(true), position(glm::vec3(0.f)), scale(glm::vec3(1.f))
{
	obj.LoadFromFileObj(filename, loadMtl);

	v_num = obj.NV();
	std::vector<Vec3<float>> vertices;
	for (int i = 0; i < v_num; ++i) {
		vertices.push_back(obj.V(i));
	}

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
}

void ObjDrawer::setProg(GLuint id)
{
	this->p_id = id;
}

void ObjDrawer::setAttrib(char const* v)
{
	v_loc = glGetAttribLocation(p_id, v);
}

void ObjDrawer::setPerspect(bool isPerspect)
{
	this->isPerspect = isPerspect;
}

void ObjDrawer::drawV()
{
	glUseProgram(p_id);
	glEnableVertexAttribArray(v_loc);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, v_num);
}

void ObjDrawer::draw(glm::mat4 v, glm::mat4 p)
{
	glUseProgram(p_id);

	glm::mat4 m = glm::translate(glm::mat4(1.f), position);
	m = glm::scale(m, scale);

	glm::mat4 mvp = p * v * m;

	GLint mvp_pos = glGetUniformLocation(p_id, "mvp");

	glUniformMatrix4fv(mvp_pos, 1, GL_FALSE, glm::value_ptr(mvp));
	
	glEnableVertexAttribArray(v_loc);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, v_num);
}

void ObjDrawer::setPosition(glm::vec3 pos)
{
	this->position = pos;
}

void ObjDrawer::setScale(glm::vec3 scale)
{
	this->scale = scale;
}
