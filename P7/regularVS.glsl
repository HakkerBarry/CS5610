#version 330 core
layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;

out vec3 v_normal;

//uniform mat4 m;
//uniform mat4 v;
uniform mat4 mv;
uniform mat4 mvp;


void main()
{
    gl_Position = mvp * vec4(VertexPos, 1.0);
    v_normal = normalize(mat3(inverse(transpose(mv)))*(VertexNormal));
    //gl_Position = vec4(0, 0, 0, 1);
}  