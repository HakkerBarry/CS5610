#version 330 core
layout (location = 0) in vec3 VertexPos;
layout (location = 1) in vec3 VertexNormal;

out vec3 w_normal;
out vec3 v_normal;
out vec3 w_camera_dir;

uniform mat4 mvp;
uniform mat4 v;
uniform mat4 mv;
uniform mat4 m;

void main()
{
    w_normal = normalize(inverse(transpose(mat3(m)))*VertexNormal);
    v_normal = normalize(inverse(transpose(mat3(mv)))*VertexNormal);
    w_camera_dir = mat3(inverse(v))*vec3(0,0,-1);
    gl_Position = mvp * vec4(VertexPos, 1.0);
}  