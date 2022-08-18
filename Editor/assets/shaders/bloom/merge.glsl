// Merge Shader

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


uniform sampler2D u_MainImage;
uniform sampler2D u_BlurImage;
uniform float u_Intensity;

void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(u_MainImage, v_TexCoord).rgb;
    vec3 bloomColor = texture(u_BlurImage, v_TexCoord).rgb * u_Intensity;

    hdrColor += bloomColor;

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor);
    // also gamma correct while we're at it       
    result = pow(result, vec3(1.0 / gamma));
    color = vec4(result, 1.0);
} 