#version 460

layout (set = 1, binding = 0) uniform Projection
{
    mat4 matrix;
} proj;

layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 tex_coords;

layout (location = 0) out vec2 out_tex_coords;

void main()
{
    gl_Position = proj.matrix * vec4(vertex_position, 1.0);
    out_tex_coords = tex_coords;
}
