// Bright Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

out vec2 v_TexCoord;

void main()
{
    gl_Position = vec4(a_Position, 1.0);
    v_TexCoord = a_TexCoord;
}

    #type fragment
    #version 460 core

layout(location = 0) out vec4 color;
in vec2 v_TexCoord;

uniform float u_Threshold;
uniform sampler2D u_MainImage;

void main()
{
    vec3 result = texture(u_MainImage, v_TexCoord).rgb;
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > u_Threshold) color = vec4(result, 1.0);
    else color = vec4(0.0, 0.0, 0.0, 1.0);
} 