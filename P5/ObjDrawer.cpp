#include <glew.h>
#include "cyVector.h"
#include "ObjDrawer.h"
#include <cmath>
#include "lodepng.h"
#include <string>

//debug
#include <iostream>

ObjDrawer::ObjDrawer(char const* filename, bool loadMtl) : v_loc(-1), isPerspect(true)
{
	obj.LoadFromFileObj(filename, loadMtl);
	prog.CreateProgram();


	int v_num = obj.NV();
	std::vector<Vec3<float>> vertices;
	std::vector<Vec2<float>> texC;

	int f_num = obj.NF();
	for (int i = 0; i < f_num; ++i) {
		vertices.push_back(obj.V(obj.F(i).v[0]));
		vertices.push_back(obj.V(obj.F(i).v[1]));
		vertices.push_back(obj.V(obj.F(i).v[2]));
		texC.push_back(Vec2<float>(obj.GetTexCoord(i, Vec3f(1, 0, 0))));
		texC.push_back(Vec2<float>(obj.GetTexCoord(i, Vec3f(0, 1, 0))));
		texC.push_back(Vec2<float>(obj.GetTexCoord(i, Vec3f(0, 0, 1))));
	}

	glUseProgram(prog.GetID());

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &TCB);
	glBindBuffer(GL_ARRAY_BUFFER, TCB);
	glBufferData(GL_ARRAY_BUFFER, texC.size() * sizeof(Vec2<float>), texC.data(), GL_STATIC_DRAW);

	if (loadMtl) {
		// texture
		glGenTextures(1, &tex);
		glBindTexture(GL_TEXTURE_2D, tex);
		std::vector<unsigned char> image; //the raw pixels
		unsigned width, height;

		unsigned error = lodepng::decode(image, width, height, "res/" + std::string(obj.M(0).map_Kd.data));
		if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, (void*)image.data());
		glGenerateMipmap(GL_TEXTURE_2D);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glBindTexture(GL_TEXTURE_2D, tex);

		GLuint sampler = glGetUniformLocation(prog.GetID(), "texc");
		glUseProgram(prog.GetID());
		glUniform1i(sampler, 0);
	}
}

void ObjDrawer::setCameraSize(int width, int height)
{
	camerWidthScale = ((float)width) / ((float)height);
}

void ObjDrawer::setTexUnit(GLint u)
{
	glUseProgram(prog.GetID());
	GLuint sampler = glGetUniformLocation(prog.GetID(), "tex");
	glUniform1i(sampler, u);
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

void ObjDrawer::setAttrib(char const* v)
{
	glUseProgram(prog.GetID());
	v_loc = glGetAttribLocation(prog.GetID(), v);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(v_loc);
	glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	tc_loc = glGetAttribLocation(prog.GetID(), "texc");
	glBindBuffer(GL_ARRAY_BUFFER, TCB);
	glEnableVertexAttribArray(tc_loc);
	glVertexAttribPointer(tc_loc, 2, GL_FLOAT, GL_FALSE, 0, 0);
}

void ObjDrawer::setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ)
{
	glUseProgram(prog.GetID());

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
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(v_loc);
	glEnableVertexAttribArray(tc_loc);
	glDrawArrays(GL_TRIANGLES, 0, obj.NF() * 3 * sizeof(Vec3f));
	glDisableVertexAttribArray(v_loc);
	glDisableVertexAttribArray(tc_loc);
}
