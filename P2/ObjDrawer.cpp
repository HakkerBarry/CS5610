#include "ObjDrawer.h"
#include <glew.h>
#include <vector>

ObjDrawer::ObjDrawer(char const* filename, bool loadMtl)
{
	obj.LoadFromFileObj(filename, loadMtl);
	prog.CreateProgram();

	
	v_num = obj.NV();
	std::vector<Vec3<float>> vertices;
	for (int i = 0; i < v_num; ++i) {
		vertices.push_back(obj.V(i));
	}

	glGenBuffers(1, &this->VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, vertices.size()*sizeof(Vec3<float>), vertices.data(), GL_STATIC_DRAW);
}

void ObjDrawer::setVS(char const* filename)
{
	vs.CompileFile("SimpleVS.glsl", GL_VERTEX_SHADER);
	prog.AttachShader(vs);
	prog.Link();
}

void ObjDrawer::setFS(char const* filename)
{
	fs.CompileFile("SimpleFS.glsl", GL_FRAGMENT_SHADER);
	prog.AttachShader(vs);
	prog.AttachShader(fs);
}

void ObjDrawer::drawPoints()
{
	glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_POINTS, 0, v_num);
}
