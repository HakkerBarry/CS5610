#version 330 core
layout (location = 0) in vec3 vert_pos;
layout (location = 2) in vec3 vert_n;

out vec3 w_normal;
out vec3 frag_pos;

uniform mat4 mvp;
uniform mat4 m;
uniform mat4 invM;
uniform mat4 v;

void main()
{
	gl_Position = mvp * vec4(vert_pos, 1.0);
	frag_pos = (mvp * vec4(vert_pos, 1.0)).xyz;
	w_normal = (inverse(transpose(m*v)) * vec4(vert_n, 0)).xyz;
}