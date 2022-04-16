#include <glew.h>
#include "ObjDrawer.h"

ObjDrawer::ObjDrawer(char const* filename, bool loadMtl) :
	v_loc(-1), isPerspect(true), position(glm::vec3(0.f)), scale(glm::vec3(1.f)), rotation(glm::vec3(0.f))
{
	obj.LoadFromFileObj(filename, loadMtl);

	v_num = obj.NV();
	int f_num = obj.NF();
	std::vector<Vec3<float>> vertices;
	std::vector<Vec2<float>> texC;
	std::vector<Vec3<float>> normals;
	std::vector<Vec3<float>> tangent;
	std::vector<Vec3<float>> bitangent;


	for (int i = 0; i < f_num; ++i) {
		vertices.push_back(obj.V(obj.F(i).v[0]));
		vertices.push_back(obj.V(obj.F(i).v[1]));
		vertices.push_back(obj.V(obj.F(i).v[2]));
		normals.push_back(obj.VN(obj.F(i).v[0]));
		normals.push_back(obj.VN(obj.F(i).v[1]));
		normals.push_back(obj.VN(obj.F(i).v[2]));
		texC.push_back(Vec2<float>(obj.GetTexCoord(i, Vec3f(1, 0, 0))));
		texC.push_back(Vec2<float>(obj.GetTexCoord(i, Vec3f(0, 1, 0))));
		texC.push_back(Vec2<float>(obj.GetTexCoord(i, Vec3f(0, 0, 1))));

		Vec3<float> E1 = obj.V(obj.F(i).v[1]) - obj.V(obj.F(i).v[0]);
		Vec3<float> E2 = obj.V(obj.F(i).v[2]) - obj.V(obj.F(i).v[0]);
		Vec2<float> deltaUV1 = Vec2<float>(obj.GetTexCoord(i, Vec3f(0, 1, 0))) - Vec2<float>(obj.GetTexCoord(i, Vec3f(1, 0, 0)));
		Vec2<float> deltaUV2 = Vec2<float>(obj.GetTexCoord(i, Vec3f(0, 0, 1))) - Vec2<float>(obj.GetTexCoord(i, Vec3f(1, 0, 0)));
		GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

		float r = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
		Vec3f tan = (E1 * deltaUV2.y - E2 * deltaUV1.y) * r;
		Vec3f bitan = (E2 * deltaUV1.x - E1 * deltaUV2.x) * r;
		tan.Normalize();
		bitan.Normalize();

		tangent.push_back(tan);
		tangent.push_back(tan);
		tangent.push_back(tan);
		bitangent.push_back(bitan);
		bitangent.push_back(bitan);
		bitangent.push_back(bitan);
	}

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &NB);
	glBindBuffer(GL_ARRAY_BUFFER, NB);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vec3<float>), normals.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &TCB);
	glBindBuffer(GL_ARRAY_BUFFER, TCB);
	glBufferData(GL_ARRAY_BUFFER, texC.size() * sizeof(Vec2<float>), texC.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &Tan);
	glBindBuffer(GL_ARRAY_BUFFER, Tan);
	glBufferData(GL_ARRAY_BUFFER, tangent.size() * sizeof(Vec3<float>), tangent.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &Bitan);
	glBindBuffer(GL_ARRAY_BUFFER, Bitan);
	glBufferData(GL_ARRAY_BUFFER, bitangent.size() * sizeof(Vec3<float>), bitangent.data(), GL_STATIC_DRAW);
}

void ObjDrawer::setProg(GLuint id)
{
	this->p_id = id;
}

void ObjDrawer::setAttrib()
{
	glUseProgram(p_id);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	v_loc = glGetAttribLocation(p_id, "m_pos");
	glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	tc_loc = glGetAttribLocation(p_id, "texc");
	glBindBuffer(GL_ARRAY_BUFFER, TCB);
	glVertexAttribPointer(tc_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);

	n_loc = glGetAttribLocation(p_id, "m_normal");
	glBindBuffer(GL_ARRAY_BUFFER, NB);
	glVertexAttribPointer(n_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	tan_loc = glGetAttribLocation(p_id, "m_tangent");
	glBindBuffer(GL_ARRAY_BUFFER, Tan);
	glVertexAttribPointer(tan_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	bitan_loc = glGetAttribLocation(p_id, "m_bitangent");
	glBindBuffer(GL_ARRAY_BUFFER, Bitan);
	glVertexAttribPointer(bitan_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
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
	glBindVertexArray(VAO);

	glm::mat4 m = glm::translate(glm::mat4(1.f), position);
	m = glm::scale(m, scale);
	m = glm::rotate(m, glm::radians(this->rotation.x), glm::vec3(1, 0, 0));
	m = glm::rotate(m, glm::radians(this->rotation.y), glm::vec3(0, 1, 0));
	m = glm::rotate(m, glm::radians(this->rotation.z), glm::vec3(0, 0, 1));

	glm::mat4 mvp = p * v * m;

	GLint mvp_pos = glGetUniformLocation(p_id, "mvp");
	glUniformMatrix4fv(mvp_pos, 1, GL_FALSE, glm::value_ptr(mvp));

	GLint m_pos = glGetUniformLocation(p_id, "m");
	glUniformMatrix4fv(m_pos, 1, GL_FALSE, glm::value_ptr(m));

	GLint v_pos = glGetUniformLocation(p_id, "v");
	glUniformMatrix4fv(v_pos, 1, GL_FALSE, glm::value_ptr(v));

	GLint p_pos = glGetUniformLocation(p_id, "p");
	glUniformMatrix4fv(p_pos, 1, GL_FALSE, glm::value_ptr(p));
	
	// Draw obj
	glEnableVertexAttribArray(v_loc);
	glEnableVertexAttribArray(n_loc);
	glEnableVertexAttribArray(tc_loc);
	glEnableVertexAttribArray(tan_loc);
	glEnableVertexAttribArray(bitan_loc);
	glDrawArrays(GL_TRIANGLES, 0, obj.NF() * 3 * sizeof(Vec3f));
	glDisableVertexAttribArray(v_loc);
	glDisableVertexAttribArray(n_loc);
	glDisableVertexAttribArray(tc_loc);
	glDisableVertexAttribArray(tan_loc);
	glDisableVertexAttribArray(bitan_loc);
}

void ObjDrawer::setPosition(glm::vec3 pos)
{
	this->position = pos;
}

void ObjDrawer::setRotation(glm::vec3 rotate)
{
	this->rotation = rotate;
}

void ObjDrawer::setScale(glm::vec3 scale)
{
	this->scale = scale;
}
