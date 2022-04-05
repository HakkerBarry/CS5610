#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 4) out;

in vec2 texCoord_GS[];
in vec3 w_normal_GS[];
in vec3 v_normal_GS[];
in vec3 w_camera_dir_GS[];
in vec3 t_camera_dir_GS[];
in vec3 t_light_GS[];

out vec2 texCoord_FS[];
out vec3 w_normal_FS[];
out vec3 v_normal_FS[];
out vec3 w_camera_dir_FS[];
out vec3 t_camera_dir_FS[];
out vec3 t_light_FS[];

void main() {    
    gl_Position = gl_in[0].gl_Position; 
    EmitVertex();

    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    
    EndPrimitive();
}  