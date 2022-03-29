#version 410 core

layout(triangles, equal_spacing, ccw) in;

in vec4 position_ES[];
in vec2 texCoord_ES[];
in vec3 w_normal_ES[];
in vec3 v_normal_ES[];
in vec3 w_camera_dir_ES[];
in vec3 t_camera_dir_ES[];
in vec3 t_light_ES[];

out vec2 texCoord_GS[];
out vec3 w_normal_GS[];
out vec3 v_normal_GS[];
out vec3 w_camera_dir_GS[];
out vec3 t_camera_dir_GS[];
out vec3 t_light_GS[];

vec4 interpolate(vec4 v0, vec4 v1, vec4 v2)
{
	//vec4 a = mix(v0, v1, gl_TessCoord.x);
	//vec4 b = mix(v3, v2, gl_TessCoord.x);
	//return mix(a, b, gl_TessCoord.y);

	return vec4(gl_TessCoord.x) * v0 + vec4(gl_TessCoord.y) * v1 + vec4(gl_TessCoord.z) * v2;
}

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
	gl_Position = interpolate(gl_in[0].gl_Position,
								gl_in[1].gl_Position,
								gl_in[2].gl_Position
								);

	texCoord_GS = interpolate2D(texCoord_ES[0],
								texCoord_ES[1],
								texCoord_ES[2]);
}