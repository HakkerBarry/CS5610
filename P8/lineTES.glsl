#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec3 pos_TES[];

out vec3 pos_GS;

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2)
{
	vec4 a = mix(v1, v0, gl_TessCoord.x);
	vec4 b = mix(v2, v0, gl_TessCoord.y);
	return mix(a, b, gl_TessCoord.z);
}

void main()
{
	gl_Position =   vec4(gl_TessCoord.x * gl_in[0].gl_Position +
 gl_TessCoord.y * gl_in[1].gl_Position +
 gl_TessCoord.z * gl_in[2].gl_Position);
}