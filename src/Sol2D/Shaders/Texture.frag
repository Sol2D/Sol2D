#version 460

layout (location = 0) in vec2 texture_coordinates;
layout (location = 1) in vec2 texture_coordinates_inverted;

layout (location = 0) out vec4 frag_color;

layout (set = 2, binding = 0) uniform sampler2D tex;
layout (set = 3, binding = 0) uniform Uniforms
{
   bool flip_h;
   bool flip_v;
} u;

void main()
{
    frag_color = texture(
        tex,
        vec2(
            u.flip_h ? texture_coordinates_inverted.x : texture_coordinates.x,
            u.flip_v ? texture_coordinates_inverted.y : texture_coordinates.y
        ));
}
