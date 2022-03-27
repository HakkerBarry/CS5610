#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec4 lightView_Pos;

uniform sampler2D tex;

void main()
{ 
    vec3 p = lightView_Pos.xyz / lightView_Pos.w;
    FragColor = vec4(1, 0, 0, 1);
    FragColor *= texture(tex, p.xy).r < p.z ? 0 : 1;
} 