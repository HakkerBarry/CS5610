#version 330 core
layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec3 m_normal;

out vec3 w_normal;

uniform mat4 mvp;
unifrom mat4 m;

void main()
{
	gl_Position = mvp * vec4(m_pos, 1.0);
	w_normal = inverse(transpose(mat3(m))) * m_normal;
}