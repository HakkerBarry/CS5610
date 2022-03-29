#version 410 core

layout(vertices = 3) out;

uniform int tes_level;

void main(void)
{
	gl_TessLevelOuter[0] = tes_level;
	gl_TessLevelOuter[1] = tes_level;
	gl_TessLevelOuter[2] = tes_level;
	//gl_TessLevelOuter[3] = tes_level;

	gl_TessLevelInner[0] = tes_level;
	//gl_TessLevelOuter[1] = 8.0;

	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}