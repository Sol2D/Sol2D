#version 460

layout(set = 1, binding = 0) uniform Uniform {
    vec4 texture_region;
    bool use_translate_to_center;
    bool use_rotate;
    uint padding0;
    uint padding1;
    mat4 mat_translate_to_center;
    mat4 mat_rotate;
    mat4 mat_translate_to_final_position;
    mat4 mat_scale;
    mat4 mat_projection;
} u;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 texture_coordinates;

layout (location = 0) out vec2 texture_coordinates_out;

void main()
{
    texture_coordinates_out = vec2(
        u.texture_region.x + texture_coordinates.x * u.texture_region.z,
        u.texture_region.y + texture_coordinates.y * u.texture_region.w
    );
    mat4 mat_model = u.mat_scale;
    if(u.use_translate_to_center)
        mat_model = u.mat_translate_to_center * mat_model;
    if(u.use_rotate)
        mat_model = u.mat_rotate * mat_model;
    mat_model = u.mat_translate_to_final_position * mat_model;
    mat4 mat_mvp = u.mat_projection * mat_model;
    gl_Position = mat_mvp * vec4(vertex_position, 1.0f);
}
