#version 330 core
layout(location=0) in vec3 vert_pos;
layout(location=1) in vec2 vert_txc;
layout(location=2) in vec3 vert_n;

out vec3 frag_n;
out vec3 frag_pos;
out vec2 frag_txc;

uniform mat4 mvp;
uniform mat4 m;
uniform mat4 invM;
//currently this shader is the same as Mesh vs 
void main(){
	gl_Position = mvp * vec4(vert_pos, 1.0);
	frag_pos = (m * vec4(vert_pos, 1.0)).xyz;
	frag_n = (invM * vec4(vert_n, 0)).xyz;
	frag_txc = vert_txc;
}