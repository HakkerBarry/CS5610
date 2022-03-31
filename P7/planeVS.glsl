#version 330 core
in vec3 pos;
in vec2 texc;

out vec2 texCoord;
out vec4 lightView_Pos;
out vec4 v_pos;

uniform mat4 m;
uniform mat4 mv;
uniform mat4 mvp;
uniform mat4 mlp;

void main()
{
	gl_Position = mvp * vec4(pos, 1.0);
	lightView_Pos = mlp * vec4(pos, 1.0);
	texCoord = texc;
	v_pos = m * vec4(pos, 1.0);
}