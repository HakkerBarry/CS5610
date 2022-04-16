#version 330 core
layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec3 gAlbedo;

in vec3 viewPos;
in vec2 TexCoords;
in vec3 v_normal;

void main()
{    
    // store the fragment position vector in the first gbuffer texture
    gPosition = viewPos;
    // also store the per-fragment normals into the gbuffer
    gNormal = normalize(v_normal);
    // and the diffuse per-fragment color
    gAlbedo.rgb = vec3(0.95);
}