// Shadow Map Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;

uniform mat4 u_Projection;
uniform mat4 u_View;

out vec3 v_WorldPos;

void main()
{
    v_WorldPos = a_Position;
    gl_Position = u_Projection * u_View * vec4(v_WorldPos, 1.0);
}

#type fragment
#version 460 core

layout(location = 0) out vec4 CubeMap0;

in vec3 v_WorldPos;
uniform sampler2D u_EquirectangularMap0;

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