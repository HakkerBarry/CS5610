#version 330 core
layout (location = 0) in vec3 VertexPos;

out vec3 dir;

uniform mat4 mvp;

void main()
{
    dir = VertexPos;
    gl_Position = mvp * vec4(VertexPos, 1.0);
}  