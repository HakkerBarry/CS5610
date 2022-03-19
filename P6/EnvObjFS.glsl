#version 330 core
layout (location = 0) out vec4 color;

in vec3 w_normal;
in vec3 w_camera_dir;
in vec3 v_normal;

uniform samplerCube env;

void main()
{
    vec3 light = vec3(-1, 1, 1);
    vec3 half = normalize((light + vec3(0, 0, 1))/length(light + vec3(0, 0, 1)));
    vec4 reflect = texture(env, reflect(w_camera_dir,w_normal));
    vec4 blinn = vec4(vec3(1, 1, 1) * pow(dot(half, v_normal), 20), 0.);

    color = vec4(max(reflect[0], blinn[0]),
    max(reflect[1], blinn[1]),
    max(reflect[2], blinn[2]),
    max(reflect[3], blinn[3])
    );
}  