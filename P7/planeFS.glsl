#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec4 lightView_Pos;
in vec4 v_pos;

uniform sampler2D tex;

void main()
{ 
    vec3 p = lightView_Pos.xyz / lightView_Pos.w;

    vec3 w_light = vec3(1/2, 1, 1);
    vec4 ambient = vec4(.1, .1, .1, 1);
    vec4 highLight = 200 * vec4(1, 1, 1, 1) / pow(distance(vec3(v_pos), 20*vec3(1/2, 1, 1)), 2);

    FragColor = ambient + highLight * ((texture(tex, p.xy).r + 0.00005) < p.z ? 0 : 1);
    //FragColor *= (texture(tex, p.xy).r + 0.00005) < p.z ? 0 : 1;
    //FragColor *= textureProj(tex, lightView_Pos);
} 