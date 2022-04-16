#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 w_normal;
in vec3 v_normal;
in vec3 w_camera_dir;
in vec3 t_camera_dir;
in vec3 t_light;

void main()
{
    //FragColor = vec4(1., 0., 0., 1.);
    FragColor = vec4(v_normal, 1.);
} 