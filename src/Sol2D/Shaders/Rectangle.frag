#version 460

layout (set = 3, binding = 0) uniform Uniforms
{
    vec4 border_color;
    vec2 resolution;
    float border_width;
} u;

layout (location = 0) in vec2 coords;

layout (location = 0) out vec4 frag_color;

void main()
{
    const vec4 fill_color = vec4(u.border_color.rgb, 0.1f);
    const float half_square_size = 0.5f;
    const vec2 center = vec2(0.5f, 0.5f);
    const float ratio = u.resolution.x / u.resolution.y;
    const float aa_size = 1.0f / u.resolution.x;
    const vec2 border_width = vec2(
                u.border_width,
                u.border_width * ratio);
    const vec2 aa = vec2(
                aa_size,
                aa_size * ratio);
    const vec2 inner_start_aa = vec2(
                half_square_size - border_width.x - aa.x,
                half_square_size - border_width.y - aa.y);
    const vec2 inner_stop_aa = inner_start_aa + aa;
    const vec2 outer_start_aa = vec2(half_square_size, half_square_size) - aa;
    const vec2 dist = vec2(
                abs(coords.x - center.x),
                abs(coords.y - center.y));

    if(dist.x < inner_start_aa.x && dist.y < inner_start_aa.y)
    {
        frag_color = fill_color;
    }
    else if(dist.x >= inner_start_aa.x && dist.x < inner_stop_aa.x && dist.y <= inner_start_aa.y + aa.y / 2)
    {
        float t = smoothstep(inner_start_aa.x, inner_stop_aa.x, dist.x);
        frag_color = mix(fill_color, u.border_color, t);
    }
    else if(dist.y >= inner_start_aa.y && dist.y < inner_stop_aa.y && dist.x <= inner_start_aa.x + aa.x / 2)
    {
        float t = smoothstep(inner_start_aa.y, inner_stop_aa.y, dist.y);
        frag_color = mix(fill_color, u.border_color, t);
    }
    else if(dist.x >= outer_start_aa.x)
    {
        float t = smoothstep(outer_start_aa.x, half_square_size + aa.x, dist.x);
        frag_color = mix(u.border_color, vec4(0.0), t);
    }
    else if(dist.y >= outer_start_aa.y)
    {
        float t = smoothstep(outer_start_aa.y, half_square_size + aa.y, dist.y);
        frag_color = mix(u.border_color, vec4(0.0), t);
    }
    else
    {
        frag_color = u.border_color;
    }
}
