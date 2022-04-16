#version 330 core
in vec3 m_pos;
in vec2 texc;
in vec3 m_normal;
in vec3 m_tangent;
in vec3 m_bitangent;

out vec2 texCoord;
out vec3 w_normal;
out vec3 v_normal;
out vec3 w_camera_dir;
out vec3 t_camera_dir;
out vec3 t_light;

uniform mat4 mvp;
uniform mat4 v;
uniform mat4 p;
uniform mat4 m;

void main()
{
	gl_Position = mvp * vec4(m_pos,  1.);
	v_normal = normalize(mat3(transpose(inverse(v*m))) * m_normal);
	//v_normal = m_normal;
}