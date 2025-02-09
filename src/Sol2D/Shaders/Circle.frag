#version 460

layout (set = 3, binding = 0) uniform Uniforms
{
    vec4 color;
    vec4 border_color;
    float border_width;
    float radius;
} u;

layout (location = 0) in vec2 coords;

layout (location = 0) out vec4 frag_color;

void renderSolid()
{
    const vec2 center = vec2(0.5f, 0.5f);
    const float radius = 0.5f;
    const float aa = 1.0f / u.radius;
    const float outer_start_aa = radius - aa;
    const float outer_stop_aa = radius;
    const float dist = distance(coords, center);

    if(dist < outer_start_aa)
    {
        frag_color = u.color;
    }
    else if(dist < outer_stop_aa)
    {
        float t = smoothstep(outer_start_aa, outer_stop_aa + aa, dist);
        frag_color = mix(u.border_color, vec4(0.0), t);
    }
    else
    {
        discard;
    }
}

void renderBordererd()
{
    const vec2 center = vec2(0.5f, 0.5f);
    const float radius = 0.5f;
    const float aa = 1.0f / u.radius;
    const float inner_start_aa = radius - u.border_width - aa;
    const float inner_stop_aa = radius - u.border_width;
    const float outer_start_aa = radius - aa;
    const float outer_stop_aa = radius;
    const float dist = distance(coords, center);

    if(dist < inner_start_aa)
    {
        frag_color = u.color;
    }
    else if(dist >= inner_start_aa && dist < inner_stop_aa)
    {
        float t = smoothstep(inner_start_aa, inner_stop_aa, dist);
        frag_color = mix(u.color, u.border_color, t);
    }
    else if(dist >= inner_stop_aa && dist < outer_start_aa)
    {
        frag_color = u.border_color;
    }
    else if(dist >= outer_start_aa && dist < outer_stop_aa)
    {
        float t = smoothstep(outer_start_aa, outer_stop_aa + aa, dist);
        frag_color = mix(u.border_color, vec4(0.0), t);
    }
    else
    {
        discard;
    }
}

void main()
{
    if(u.border_width == 0.0f)
        renderSolid();
    else
        renderBordererd();
}
