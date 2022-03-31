#version 330 core
layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;

out vec3 w_normal;
out vec4 lightView_Pos;
out vec3 w_camera_dir;

uniform mat4 m;
uniform mat4 v;
uniform mat4 mv;
uniform mat4 mvp;

uniform mat4 mlp;


void main()
{
    gl_Position = mvp * vec4(VertexPos, 1.0);
    //gl_Position = mlp * vec4(VertexPos, 1.0);
    w_normal = normalize(mat3(inverse(transpose(m)))*(VertexNormal));
    lightView_Pos = mlp * vec4(VertexPos, 1.0);
    w_camera_dir = inverse(mat3(v)) * vec3(0, 0, 1);
}  