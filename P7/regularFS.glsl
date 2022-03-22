#version 330 core
layout (location = 0) out vec4 color;
in vec3 v_normal;

void main()
{
    color = vec4((v_normal), 1);
}  