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
	prog.CreateProgram();


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
	}

	glUseProgram(prog.GetID());

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &NB);
	glBindBuffer(GL_ARRAY_BUFFER, NB);
	glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(Vec3<float>), normals.data(), GL_STATIC_DRAW);

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

void ObjDrawer::setTexUnit(GLint u)
{
	glUseProgram(prog.GetID());
	GLuint sampler = glGetUniformLocation(prog.GetID(), "tex");
	glUniform1i(sampler, u);
}

void ObjDrawer::setShader(char const* vs_path, char const* fs_path)
{
	vs.CompileFile(vs_path, GL_VERTEX_SHADER);
	fs.CompileFile(fs_path, GL_FRAGMENT_SHADER);
	prog.AttachShader(vs);
	prog.AttachShader(fs);
	prog.Link();
}

void ObjDrawer::setAttrib()
{
	glUseProgram(prog.GetID());
	v_loc = glGetAttribLocation(prog.GetID(), "VertexPos");
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glEnableVertexAttribArray(v_loc);
	glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);

	n_loc = glGetAttribLocation(prog.GetID(), "VertexNormal");
	glBindBuffer(GL_ARRAY_BUFFER, NB);
	glEnableVertexAttribArray(n_loc);
	glVertexAttribPointer(n_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
}

void ObjDrawer::setMV(float rotateX, float rotateY, float rotateZ, float scale, float transformZ)
{
	glUseProgram(prog.GetID());

	Matrix4<float> m_model;
	m_model.SetRotationX(-1.57f);
	Matrix4<float> m_view;
	m_view.SetRotationZYX(rotateX, rotateY, rotateZ);
	Matrix4<float> v_trans;
	v_trans.SetTranslation(Vec3f(0., 0., -50 - 10*transformZ));
	m_view = v_trans * m_view;

	Matrix4<float> m_pres;
	m_pres.SetPerspective(1.f, 1920. / 1080., 0.1f, 100.f);

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

	GLint mv_pos = glGetUniformLocation(prog.GetID(), "mv");
	glUniformMatrix4fv(mv_pos, 1, false, mv_sending);

	GLint mvp_pos = glGetUniformLocation(prog.GetID(), "mvp");
	glUniformMatrix4fv(mvp_pos, 1, false, sending);
}

void ObjDrawer::setMVP(Matrix4<float> m, Matrix4<float> v, Matrix4<float> p)
{
	Matrix4<float> m_model;
	m_model.SetRotationX(-1.57f);
	Matrix4<float> mvp = p * v * m_model;
	float mvp_sending[16];
	mvp.Get(mvp_sending);
	GLint mvp_pos = glGetUniformLocation(prog.GetID(), "mvp");
	glUniformMatrix4fv(mvp_pos, 1, false, mvp_sending);

	for(int i = 0; i < 16; i++)
		std::cout << mvp_sending[i] <<" ";
}


void ObjDrawer::resetGLProg()
{
	prog.Bind();
	glDeleteProgram(prog.GetID());
}



void ObjDrawer::drawV()
{
	prog.Bind();
	glDrawArrays(GL_POINTS, 0, obj.NV());
}

void ObjDrawer::drawTri()
{
	glUseProgram(prog.GetID());
	glBindVertexArray(VAO);
	glEnableVertexAttribArray(v_loc);
	glDrawArrays(GL_TRIANGLES, 0, obj.NF() * 3 * sizeof(Vec3f));
	glDisableVertexAttribArray(v_loc);
}
