#version 330 core
out vec4 FragColor;

uniform vec4 cam_dir;
uniform vec3 light_dir;

in vec3 Normal;

void main()
{
    vec3 CamDir = normalize(vec3(cam_dir.x, cam_dir.y, cam_dir.z));
    FragColor = dot(Normal, normalize(light_dir)) * vec4(1, 0, 0, 1);
} 