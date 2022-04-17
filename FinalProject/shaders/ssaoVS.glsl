#version 330 core
layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec2 texc;

out vec2 TexCoords;

void main()
{
    TexCoords = texc;
    gl_Position = vec4(m_pos, 1.0);
}