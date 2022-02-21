#version 330 core
out vec4 FragColor;

uniform sampler2D tex;

void main()
{
    //FragColor = texture(tex, texCoord);
    FragColor = vec4(1, 0, 0, 1);
} 