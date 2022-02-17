#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 normal;

out vec3 Normal;

uniform mat4 mvp;
uniform mat4 n_mv;
uniform vec4 cam_dir;
uniform vec3 light_dir;

void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
	Normal = normalize(vec3(n_mv * vec4(normal, 0)));
}