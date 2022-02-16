#version 330 core
out vec4 FragColor;

in vec4 Normal;

void main()
{
    FragColor = Normal;
} 