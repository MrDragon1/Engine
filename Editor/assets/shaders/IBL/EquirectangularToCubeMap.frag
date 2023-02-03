#version 460 core

layout(location = 0) out vec4 CubeMap0;

layout(location = 0) in vec3 v_WorldPos;
layout(binding = 0) uniform sampler2D u_EquirectangularMap0;

const vec2 invAtan = vec2(0.1591, 0.3183);
vec2 SampleSphericalMap(vec3 v)
{
    vec2 uv = vec2(atan(v.z, v.x), asin(v.y));
    uv *= invAtan;
    uv += 0.5;
    return uv;
}

void main()
{
    vec2 uv = SampleSphericalMap(normalize(v_WorldPos));
    vec3 color0 = texture(u_EquirectangularMap0, uv).rgb;

    CubeMap0 = vec4(color0, 1.0);

}