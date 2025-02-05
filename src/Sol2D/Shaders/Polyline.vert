#version 460

layout (set = 1, binding = 0) uniform Uniforms
{
    vec2 viewport_size;
} u;

layout (location = 0) in vec2 vertex_position;

void main()
{
    const float h_scale = 2.0f / u.viewport_size.x;
    const float v_scale = 2.0f / u.viewport_size.y;
    gl_Position = vec4(
        vertex_position.x * h_scale - 1.0f,
        1.0f - vertex_position.y * v_scale,
        .0f,
        1.0f);
}
