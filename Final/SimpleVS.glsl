#version 330 core
layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec3 normal;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(m_pos, 1.0);
}