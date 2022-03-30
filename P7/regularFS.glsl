#version 330 core
layout (location = 0) out vec4 color;

in vec3 v_normal;
in vec4 lightView_Pos;

uniform sampler2D tex;

void main()
{
    vec3 p = lightView_Pos.xyz / lightView_Pos.w;
    //color = vec4((v_normal), 1);
    color = vec4(1, 0, 0, 1);
    color *= (texture(tex, p.xy).r + 0.000002) < p.z ? 0 : 1;
    //color = lightView_Pos;
}  