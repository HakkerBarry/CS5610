#version 330 core
out vec4 FragColor;

uniform vec4 cam_dir;
uniform vec3 light_dir;

in vec3 Normal;
in vec2 texCoord;

uniform sampler2D tex;

void main()
{
    vec3 CamDir = normalize(vec3(cam_dir.x, cam_dir.y, cam_dir.z));
    vec3 h = (CamDir + normalize(light_dir)) / normalize(CamDir + normalize(light_dir));
    float phi = dot(normalize(h), Normal);
    vec4 diffuse = dot(Normal, normalize(light_dir)) * vec4(1, 0, 0, 1);
    vec4 ref = pow(phi, 15) * vec4(1, 1, 1, 1);
    //FragColor = diffuse + ref;
    FragColor = texture(tex, texCoord);
} 