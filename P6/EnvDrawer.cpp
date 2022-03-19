#include <glew.h>
#include "EnvDrawer.h"
#include "lodepng.h"
#include <iostream>
#include <string>

EnvDrawer::EnvDrawer(char const* filename, char const* vs_path, char const* fs_path)
{
	obj.LoadFromFileObj(filename, false);

	unsigned width, height;

	// enviroment texture
	std::vector<std::vector<unsigned char>> env(6);

	// Decode textures and generate on GPU
	glActiveTexture(GL_TEXTURE0);
	unsigned error = lodepng::decode(env[0], width, height, "res/Cube/" + std::string("cubemap_negx.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glGenTextures(1, &texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[0].data());

	error = lodepng::decode(env[1], width, height, "res/Cube/" + std::string("cubemap_negy.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[1].data());

	error = lodepng::decode(env[2], width, height, "res/Cube/" + std::string("cubemap_negz.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[2].data());

	error = lodepng::decode(env[3], width, height, "res/Cube/" + std::string("cubemap_posx.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[3].data());

	error = lodepng::decode(env[4], width, height, "res/Cube/" + std::string("cubemap_posy.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[4].data());

	error = lodepng::decode(env[5], width, height, "res/Cube/" + std::string("cubemap_posz.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[5].data());

	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	// Gen Cube Buffer
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	int v_num = obj.NV();
	std::vector<Vec3<float>> vertices;

	int f_num = obj.NF();
	for (int i = 0; i < f_num; ++i) {
		vertices.push_back(obj.V(obj.F(i).v[0]));
		vertices.push_back(obj.V(obj.F(i).v[1]));
		vertices.push_back(obj.V(obj.F(i).v[2]));
	}

	prog.CreateProgram();
	vs.CompileFile(vs_path, GL_VERTEX_SHADER);
	fs.CompileFile(fs_path, GL_FRAGMENT_SHADER);
	prog.AttachShader(vs);
	prog.AttachShader(fs);
	prog.Link();
	glUseProgram(prog.GetID());


	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
	v_loc = glGetAttribLocation(prog.GetID(), "VertexPos");
	glVertexAttribPointer(v_loc, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glEnableVertexAttribArray(v_loc);

	mvp_loc = glGetUniformLocation(prog.GetID(), "mvp");

	GLuint sampler = glGetUniformLocation(prog.GetID(), "env");
	glUniform1i(sampler, 0);
}

void EnvDrawer::setMVP(float rotateX, float rotateY, float rotateZ)
{
	glUseProgram(prog.GetID());
	cyMatrix4f view;
	view.SetRotationZYX(rotateX, rotateY, rotateZ);
	cyMatrix4f pres;
	pres.SetPerspective(1.f, 1920./1080., 0.1f, 100.f);
	cyMatrix4f mvp = pres * view;
	float sending[16];
	mvp.Get(sending);
	glUniformMatrix4fv(mvp_loc, 1, false, sending);
}

void EnvDrawer::drawEnv()
{
	glDepthMask(GL_FALSE);
	glUseProgram(prog.GetID());
	glBindVertexArray(VAO);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID);
	glEnableVertexAttribArray(v_loc);
	glDrawArrays(GL_TRIANGLES, 0, 12 * 3 * sizeof(Vec3f));
	glDisableVertexAttribArray(v_loc);
	glDepthMask(GL_TRUE);
}

GLuint EnvDrawer::getEnvTex()
{
	return texID;
}
