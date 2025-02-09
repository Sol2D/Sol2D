#version 460

layout(set = 1, binding = 0) uniform Uniform {
    mat4 mat_translate_to_final_position;
    mat4 mat_scale;
    mat4 mat_projection;
} u;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 texture_coordinates;

layout (location = 0) out vec2 texture_coordinates_out;

void main()
{
    gl_Position =
        u.mat_projection *
        u.mat_translate_to_final_position *
        u.mat_scale *
        vec4(vertex_position, 1.0f);
    texture_coordinates_out = texture_coordinates;
}
