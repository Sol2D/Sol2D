#version 460

layout (location = 0) in vec2 texture_coordinates;

layout (location = 0) out vec4 frag_color;

layout (set = 2, binding = 0) uniform sampler2D tex;

void main(void)
{
    frag_color = texture(tex, texture_coordinates);
}
