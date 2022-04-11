#version 330 core

in vec3 w_normal;

out vec4 FragColor;

void main()
{
    //FragColor = diffuse + ref;
    FragColor = vec4(gl_position, 0);
    FragColor = vec4(w_normal, 0);
} 