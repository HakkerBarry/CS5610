#include <glew.h>
#include "cyVector.h"
#include "ObjDrawer.h"
#include <cmath>
#include "lodepng.h"
#include <string>

#include <iostream>

ObjDrawer::ObjDrawer(char const* filename, bool loadMtl) : v_loc(-1)
{
	obj.LoadFromFileObj(filename, loadMtl);


	int v_num = obj.NV();
	std::vector<Vec3<float>> vertices;
	std::vector<Vec2<float>> texC;
	std::vector<Vec3<float>> normals;

	int f_num = obj.NF();
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
}

void ObjDrawer::setTexUnit(GLint u)
{
	glUseProgram(prog_id);
	GLuint sampler = glGetUniformLocation(prog_id, "tex");
	glUniform1i(sampler, u);
}

void ObjDrawer::setProg(GLuint prog)
{
	prog_id = prog;
}

void ObjDrawer::setAttrib()
{
	glUseProgram(prog_id);
	v_loc = glGetAttribLocation(prog_id, "VertexPos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(v_loc);
	glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	n_loc = glGetAttribLocation(prog_id, "VertexNormal");
	glBindBuffer(GL_ARRAY_BUFFER, NB);
	glEnableVertexAttribArray(n_loc);
	glVertexAttribPointer(n_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void ObjDrawer::setAttribPlane()
{
	glUseProgram(prog_id);
	v_loc = glGetAttribLocation(prog_id, "pos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(v_loc);
	glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	tc_loc = glGetAttribLocation(prog_id, "texc");
	glBindBuffer(GL_ARRAY_BUFFER, TCB);
	glEnableVertexAttribArray(tc_loc);
	glVertexAttribPointer(tc_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void ObjDrawer::setCameraSize(int width, int height)
{
	camerWidthScale = ((float)width) / ((float)height);
}

Matrix4<float> ObjDrawer::setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ)
{
	glUseProgram(prog_id);

	Matrix4<float> m_model;
	m_model.SetRotationX(-1.57f);
	Matrix4<float> m_view;
	m_view.SetRotationZYX(rotateX, rotateY, rotateZ);
	Matrix4<float> v_trans;
	v_trans.SetTranslation(Vec3f(0., 0., -50 - 10*transformZ));
	m_view = v_trans * m_view;

	Matrix4<float> m_pres;
	m_pres.SetPerspective(1.f, camerWidthScale, 0.1f, 1000.f);

	Matrix4<float> mv = m_view * m_model;
	Matrix4<float> v = m_view;
	mvp = m_pres * m_view * m_model;
	Matrix4<float> m = m_model;

	float sending[16];
	mvp.Get(sending);
	float mv_sending[16];
	mv.Get(mv_sending);

	float v_sending[16];
	v.Get(v_sending);

	float m_sending[16];
	m.Get(m_sending);

	GLint mv_pos = glGetUniformLocation(prog_id, "mv");
	glUniformMatrix4fv(mv_pos, 1, false, mv_sending);

	GLint mvp_pos = glGetUniformLocation(prog_id, "mvp");
	glUniformMatrix4fv(mvp_pos, 1, false, sending);

	Matrix4<float> shadow_scale;
	shadow_scale.SetScale(0.5);
	Matrix4<float> shadow_trans;
	shadow_scale.SetTranslation(Vec3f(0.5, 0.5, 0.5));
	return shadow_trans * shadow_scale * mvp;
}

void ObjDrawer::setMLP(float* MLP)
{
	GLint mlp_pos = glGetUniformLocation(prog_id, "mlp");
	glUniformMatrix4fv(mlp_pos, 1, false, MLP);
}


void ObjDrawer::resetGLProg()
{
	glUseProgram(prog_id);
	glDeleteProgram(prog_id);
}



void ObjDrawer::drawV()
{
	glUseProgram(prog_id);
	glDrawArrays(GL_POINTS, 0, obj.NV());
}

void ObjDrawer::drawTri()
{
	glUseProgram(prog_id);
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(v_loc);
	glDrawArrays(GL_TRIANGLES, 0, obj.NF() * 3 * sizeof(Vec3f));
	glDisableVertexAttribArray(v_loc);
}
