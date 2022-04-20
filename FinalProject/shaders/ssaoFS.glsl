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
    // create TBN change-of-basis matrix: from tangent-space to view-space
    vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    // iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // for each sample
        // get sample position
        vec3 samplePos = TBN * samples[i]; // from tangent to view space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = p * offset; // from view to clip space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
        
        // get sample depth
        float sampleDepth = texture(gPosition, offset.xy).z; // get depth value of kernel sample
        
        // range check & accumulate
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