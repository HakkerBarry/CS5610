#version 330 core
layout (location = 0) in vec3 pos;
in vec2 texc;

out vec2 texCoord;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
	texCoord = texc;
}