#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedo;
uniform sampler2D ssao;

uniform bool ssaoOn;

void main()
{             
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedo, TexCoords).rgb;
    float AmbientOcclusion = texture(ssao, TexCoords).r;

    // Const
    vec3 lightDir = normalize(vec3(1, 1, 1));
    vec3 objectColor = vec3(0.7, 0.7, 0.7);
    vec3 specColor = vec3(.9, .9, .9);
    vec3 ambColor = vec3(0.3, 0.3, 0.3);

    vec3 camDir  = normalize(-FragPos);

    vec3 half = normalize(lightDir + camDir);

    float spec = pow(max(dot(Normal, half), 0.0), 16.0);

    float cosT = max(dot(Normal, lightDir), 0.0);

    vec3 color = ambColor + objectColor * cosT + spec * specColor;

    if(ssaoOn)
        FragColor = vec4(color * AmbientOcclusion, 1.0);
    else
        FragColor = vec4(color, 1.0);
}