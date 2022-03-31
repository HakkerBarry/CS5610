#version 330 core
layout (location = 0) out vec4 color;

in vec3 w_normal;
in vec4 lightView_Pos;
in vec3 v_camera_dir;

uniform sampler2D tex;

void main()
{
    vec3 p = lightView_Pos.xyz / lightView_Pos.w;

    vec3 w_light = vec3(1/2, 1, 1);
    vec4 ambient = vec4(.2, 0, 0, 1);
    vec3 diffuse = pow(max(dot(w_light, w_normal), 0), (texture(tex, p.xy).r + 0.000002) > p.z ? 5 : 100) * vec3(1, 0, 0);
    vec3 half = normalize((w_light + v_camera_dir)/length(w_light + v_camera_dir));
    vec4 blinn = max(vec4(vec3(1, 1, 1) * pow(dot(half, w_normal), 25), 0.), 0);

    //color = vec4((v_normal), 1);
    color = ambient + vec4(diffuse, 0) + blinn;
    //color = vec4(1, 0, 0, 1);
    //color *= (texture(tex, p.xy).r + 0.000002) < p.z ? 0 : 1;
    //color = lightView_Pos;
}  