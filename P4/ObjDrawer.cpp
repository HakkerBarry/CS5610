#include <glew.h>
#include "cyVector.h"
#include "ObjDrawer.h"
#include <cmath>
#include <iostream>

ObjDrawer::ObjDrawer(char const* filename, bool loadMtl) : v_loc(-1), isPerspect(true)
{
	obj.LoadFromFileObj(filename, loadMtl);
	prog.CreateProgram();


	int v_num = obj.NV();
	std::vector<Vec3<float>> vertices;
	std::vector<Vec3<float>> normals;
	for (int i = 0; i < v_num; ++i) {
		vertices.push_back(obj.V(i));
		normals.push_back(obj.VN(i));
	}

	int f_num = obj.NF();
	std::vector<TriMesh::TriFace> faces;
	for (int i = 0; i < f_num; ++i) {
		faces.push_back(obj.F(i));
	}

	GLuint VAO;
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &tris);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tris);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, f_num * sizeof(TriMesh::TriFace), faces.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &NB);
	glBindBuffer(GL_ARRAY_BUFFER, NB);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), normals.data(), GL_STATIC_DRAW);

	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
	std::cout << obj.M(0).map_Kd << std::endl;
	//glTexImage2D();
}

void ObjDrawer::setCameraSize(int width, int height)
{
	camerWidthScale = ((float)width) / ((float)height);
}

void ObjDrawer::setVS(char const* filename)
{
	vs.CompileFile(filename, GL_VERTEX_SHADER);
	prog.AttachShader(vs);
	prog.Link();
}

void ObjDrawer::setFS(char const* filename)
{
	fs.CompileFile(filename, GL_FRAGMENT_SHADER);
	prog.AttachShader(fs);
	prog.Link();
}

void ObjDrawer::setAttrib(char const* v, char const* n)
{
	v_loc = glGetAttribLocation(prog.GetID(), v);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(v_loc);
	glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	n_loc = glGetAttribLocation(prog.GetID(), n);
	glBindBuffer(GL_ARRAY_BUFFER, NB);
	glEnableVertexAttribArray(n_loc);
	glVertexAttribPointer(n_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
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

	GLint mvp_pos = glGetUniformLocation(prog.GetID(), "n_mv");
	Matrix4<float> m_orth = m_rotateX * m_rotateY * m_rotateZ * m_scale;
	float sending[16];
	m_orth.Get(sending);
	glUniformMatrix4fv(mvp_pos, 1, false, sending);

	GLint cam_pos = glGetUniformLocation(prog.GetID(), "cam_dir");
	Vec4f cam = mvp.Column(2);
	float camera[4];
	cam.Get(camera);
	glUniform4fv(cam_pos, 1, camera);

	GLint light_pos = glGetUniformLocation(prog.GetID(), "light_dir");
	float l[3] = { 1, 1, 1 };
	glUniform3fv(light_pos, 1, l);

	sendMVP();
}

void ObjDrawer::setPerspect(bool isPerspect)
{
	this->isPerspect = isPerspect;
}

void ObjDrawer::resetGLProg()
{
	prog.Bind();
	glDeleteProgram(prog.GetID());
}

void ObjDrawer::sendMVP()
{
	prog.Bind();
	GLint mvp_pos = glGetUniformLocation(prog.GetID(), "mvp");
	float sending[16];

	// 2/(r - l)  0				0				-(r + l)/(r - l)
	//0				2/(t-b)		0				-(t + b)/(r - l)
	//0				0			2/(n - f)		-(n + f)/(n - f)
	//0				0			0
	Matrix4<float> m_orth({ 1.f / camerWidthScale, 0, 0, 0, 0, 1.f, 0, 0, 0, 0, 1.f / 100, 0, 0, 0, 0, 1. });

	if (isPerspect) {
		Matrix4f m_pres = Matrix4f::Perspective(1.6f, camerWidthScale, 0.1f, 100.f);
		//Matrix4<float> m_pres({ -1, 0, 0, 0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0 });
		(m_pres * mvp).Get(sending);
	}
	else {
		mvp.Get(sending);
	}

	glUniformMatrix4fv(mvp_pos, 1, false, sending);
}

void ObjDrawer::drawV()
{
	prog.Bind();
	glDrawArrays(GL_POINTS, 0, obj.NV());
}

void ObjDrawer::drawTri()
{
	prog.Bind();
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, tris);
	glDrawElements(GL_TRIANGLES, obj.NF() * sizeof(TriMesh::TriFace), GL_UNSIGNED_INT, 0);
}
