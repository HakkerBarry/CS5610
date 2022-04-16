#version 330 core
layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec3 m_normal;
layout (location = 2) in vec2 texc;

out vec3 viewPos;
out vec2 TexCoords;
out vec3 v_normal;

//uniform bool invertedNormals;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 mvp;

void main()
{
    viewPos = (v * m * vec4(m_pos, 1.0)).xyz;
    TexCoords = texc;
    
    mat3 normalMatrix = transpose(inverse(mat3(v * m)));
    v_normal = normalMatrix * m_normal;
    
    gl_Position = mvp * vec4(m_pos, 1.0);
}