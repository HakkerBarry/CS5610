#version 330 core
out float FragColor;

in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D texNoise;

uniform vec3 samples[64];

int kernelSize = 64;
float radius = 0.6;
float bias = 0.005;

// How many noise in the screen
const vec2 noiseScale = vec2(1920.0/4.0, 1080.0/4.0); 

uniform mat4 p;

void main()
{
    // get data from texture
    vec3 fragPos = texture(gPosition, TexCoords).xyz;
    vec3 normal = normalize(texture(gNormal, TexCoords).rgb);
    vec3 randomVec = normalize(texture(texNoise, TexCoords * noiseScale).xyz);
    // get TBN matrix tangent space to view space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);

    // For each sample
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // reletive distance from tangent to view space
        vec3 samplePos = TBN * samples[i]; 
        // get sample position in view space 
        samplePos = fragPos + samplePos * radius; 
        
        vec4 offset = vec4(samplePos, 1.0);
        offset = p * offset;
        offset.xyz /= offset.w;
        offset.xyz = offset.xyz * 0.5 + 0.5;
        
        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z;
        
        // depth test
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;           
    }

    // inverse and scale to 0 - 1
    occlusion = 1.0 - (occlusion / kernelSize);
    
    FragColor = occlusion;

    // for test
    //FragColor =  vec4(bitangent, 1);
    //FragColor = vec4(occlusion, occlusion, occlusion, 1);
    //FragColor = vec4(1, 0, 0, 1);
}