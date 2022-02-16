#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec4 Normal;

uniform mat4 mvp;
uniform mat4 n_mv;

void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
	Normal = normalize(n_mv * vec4(normal, 0));
}