#version 460

layout (set = 3, binding = 0) uniform Uniforms
{
    vec4 color;
} u;

layout (location = 0) out vec4 frag_color;

void main()
{
    frag_color = u.color;
}
