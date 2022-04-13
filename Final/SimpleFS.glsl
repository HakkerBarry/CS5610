#version 330 core

in vec3 w_normal;
in vec3 frag_pos;

layout (location = 0) out vec4 FragColor;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedo;

void main()
{
    //FragColor = diffuse + ref;
    FragColor = vec4(frag_pos.xy, 0, 0);
    //vec3 normal = normalize(w_normal);
    gNormal = normalize(w_normal);
    gAlbedo.rgb = vec3(0.95);
} 