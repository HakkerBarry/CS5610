#version 330 core

in vec3 w_normal;
in vec3 frag_pos;

out vec4 FragColor;

void main()
{
    //FragColor = diffuse + ref;
    //FragColor = vec4(frag_pos.xy, 0, 0);
    //vec3 normal = normalize(w_normal);
    //FragColor = vec4(normal, 1);
    FragColor = vec4(vec3(0.95), 1);
} 