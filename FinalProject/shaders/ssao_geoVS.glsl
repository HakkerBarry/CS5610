#version 330 core
layout (location = 0) in vec3 m_pos;
layout (location = 1) in vec3 m_normal;
layout (location = 2) in vec2 texc;

out vec3 FragPos;
out vec2 TexCoords;
out vec3 Normal;

uniform bool invertedNormals;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform mat4 mvp;

void main()
{
    vec4 viewPos = view * model * vec4(aPos, 1.0);
    FragPos = viewPos.xyz; 
    TexCoords = aTexCoords;
    
    mat3 normalMatrix = transpose(inverse(mat3(view * model)));
    Normal = normalMatrix * (invertedNormals ? -aNormal : aNormal);
    
    gl_Position = projection * viewPos;
}