#version 460

layout (set = 1, binding = 0) uniform Uniform
{
    mat4 projection_matrix;
    vec4 texture_region;
} u;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 texture_coordinates;

layout (location = 0) out vec2 texture_coordinates_out;

void main()
{
    gl_Position = u.projection_matrix * vec4(vertex_position, 1.0);
    texture_coordinates_out = vec2(
        u.texture_region.x + texture_coordinates.x * u.texture_region.z,
        u.texture_region.y + texture_coordinates.y * u.texture_region.w
    );
}
