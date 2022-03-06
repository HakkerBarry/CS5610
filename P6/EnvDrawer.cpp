#include <glew.h>
#include "EnvDrawer.h"
#include "lodepng.h"
#include <iostream>
#include <string>

EnvDrawer::EnvDrawer(char const* filename)
{
	obj.LoadFromFileObj(filename, false);

	unsigned width, height;

	// enviroment texture
	std::vector<std::vector<unsigned char>> env;

	// Decode textures and generate on GPU
	unsigned error = lodepng::decode(env[0], width, height, "res/" + std::string("cubemap_negx.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glGenTextures(1, texID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[0]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[0].data());

	unsigned error = lodepng::decode(env[1], width, height, "res/" + std::string("cubemap_negy.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glGenTextures(1, texID+1);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[1]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[1].data());

	unsigned error = lodepng::decode(env[2], width, height, "res/" + std::string("cubemap_negz.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glGenTextures(1, texID+2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[2]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[2].data());

	unsigned error = lodepng::decode(env[3], width, height, "res/" + std::string("cubemap_posx.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glGenTextures(1, texID+3);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[3]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[3].data());

	unsigned error = lodepng::decode(env[4], width, height, "res/" + std::string("cubemap_posy.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glGenTextures(1, texID+4);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[4]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[4].data());

	unsigned error = lodepng::decode(env[5], width, height, "res/" + std::string("cubemap_posz.png"));
	if (error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
	glGenTextures(1, texID+5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texID[5]);
	glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, env[5].data());


	// Gen Cube Buffer
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

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

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
	glGenBuffers(1, &TCB);
	glBindBuffer(GL_ARRAY_BUFFER, TCB);
	glBufferData(GL_ARRAY_BUFFER, texC.size() * sizeof(Vec2<float>), texC.data(), GL_STATIC_DRAW);
}

void EnvDrawer::drawEnv()
{
	glBindVertexArray(VAO);
	
}
