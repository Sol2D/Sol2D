#version 460

layout (set = 3, binding = 0) uniform Uniforms
{
    vec4 color;
    vec4 border_color;
    vec2 resolution;
    float border_width;
    float radius;
} u;

layout (location = 0) in vec2 coords;

layout (location = 0) out vec4 frag_color;

const float radius = 0.5f;

void setBorderedColor(float _dist_to_center)
{
    const float aa = 1.0f / min(u.resolution.x, u.resolution.y);
    const float inner_start_aa = radius - u.border_width - aa;
    const float inner_stop_aa = radius - u.border_width;
    const float outer_start_aa = radius - aa;
    const float outer_stop_aa = radius;

    if(_dist_to_center < inner_start_aa)
    {
        frag_color = u.color;
    }
    else if(_dist_to_center >= inner_start_aa && _dist_to_center < inner_stop_aa)
    {
        float t = smoothstep(inner_start_aa, inner_stop_aa, _dist_to_center);
        frag_color = mix(u.color, u.border_color, t);
    }
    else if(_dist_to_center >= inner_stop_aa && _dist_to_center < outer_start_aa)
    {
        frag_color = u.border_color;
    }
    else if(_dist_to_center >= outer_start_aa && _dist_to_center < outer_stop_aa)
    {
        float t = smoothstep(outer_start_aa, outer_stop_aa + aa, _dist_to_center);
        frag_color = mix(u.border_color, vec4(0.0), t);
    }
    else
    {
        discard;
    }
}

void setSolidColor(float _dist_to_center)
{
    const float aa = 1.0f / min(u.resolution.x, u.resolution.y);
    const float outer_start_aa = radius - aa;
    const float outer_stop_aa = radius;

    if(_dist_to_center < outer_start_aa)
    {
        frag_color = u.color;
    }
    else if(_dist_to_center >= outer_start_aa && _dist_to_center < outer_stop_aa)
    {
        float t = smoothstep(outer_start_aa, outer_stop_aa + aa, _dist_to_center);
        frag_color = mix(u.color, vec4(0.0), t);
    }
    else
    {
        discard;
    }
}

void setColor(float _dist_to_center)
{
    if(u.border_width == 0.0f)
        setSolidColor(_dist_to_center);
    else
        setBorderedColor(_dist_to_center);
}

void main()
{
    const float ratio = u.resolution.y / u.resolution.x;
    const vec2 point = vec2(coords.x, coords.y * ratio);
    const vec2 center1 = vec2(radius, 1.0f * ratio - radius);
    const vec2 center2 = vec2(radius, radius);

    if(point.y <= center1.y && point.y >= center2.y)
    {
        //
        // Inside a rectangle
        //

        setColor(max(radius - coords.x, coords.x - radius));
        return;
    }

    float dist_c1 = distance(point, center1);
    float dist_c2 = distance(point, center2);
    if(dist_c1 <= radius)
    {
        //
        // Inside a circle 1
        //

        setColor(dist_c1);
    }
    else if(dist_c2 <= radius)
    {
        //
        // Inside a circle 2
        //

        setColor(dist_c2);
    }
    else
    {
        discard;
    }
}
