#include <glew.h>
#include "cyVector.h"
#include "ObjDrawer.h"
#include <cmath>

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

void ObjDrawer::setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ)
{
	float rx[] = { 1, 0, 0, 0, 0, cos(rotateX), sin(rotateX), 0, 0, -sin(rotateX), cos(rotateX), 0, 0, 0, 0, 1 };
	Matrix4<float> m_rotateX(rx);

	float ry[] = { cos(rotateY), 0, -sin(rotateY), 0, 0, 1, 0, 0, sin(rotateY), 0, cos(rotateY), 0, 0, 0, 0, 1 };
	Matrix4<float> m_rotateY(ry);

	float rz[] = { cos(rotateZ), sin(rotateZ), 0, 0, -sin(rotateZ), cos(rotateZ), 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 };
	Matrix4<float> m_rotateZ(rz);

	float s[] = { scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, scale, 0, 0, 0, 0, 1 };
	Matrix4<float> m_scale(s);

	float t[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, transformZ, 1 };
	Matrix4<float> m_trans(t);

	float t2[] = { 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, -.3, 1 };
	Matrix4<float> m_trans2(t2);

	mvp = m_trans * m_rotateX * m_rotateY * m_rotateZ * m_trans2 * m_scale;

	sendMVP();
}

void ObjDrawer::setPerspect(bool isPerspect)
{
	this->isPerspect = isPerspect;
}

void ObjDrawer::sendMVP()
{
	glUseProgram(p_id);
	GLint mvp_pos = glGetUniformLocation(p_id, "mvp");
	float sending[16];
	if (isPerspect) {
		Matrix4<float> m_pres({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, -1, 0 });
		//Matrix4<float> m_pres({ 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1 });
		(m_pres * mvp).Get(sending);
	}
	else {
		mvp.Get(sending);
	}

	glUniformMatrix4fv(mvp_pos, 1, false, sending);
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
