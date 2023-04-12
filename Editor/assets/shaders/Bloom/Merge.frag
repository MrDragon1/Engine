#version 460 core

layout(location = 0) out vec4 color;
layout(location = 0) in vec2 v_TexCoord;


layout(binding = 0) uniform sampler2D u_MainImage;
layout(binding = 1) uniform sampler2D u_BlurImage;

layout(std140, binding = 0) uniform Config{
    float Threshold;
    float Knee;
    float Intensity;
    int DownSample;
    int MipLevel;
}u_Config;


void main()
{
    const float gamma = 2.2;
    vec3 hdrColor = texture(u_MainImage, v_TexCoord).rgb;
    vec3 bloomColor = texture(u_BlurImage, v_TexCoord).rgb * u_Config.Intensity;

    hdrColor += bloomColor;

    // tone mapping
    vec3 result = vec3(1.0) - exp(-hdrColor);
    // also gamma correct while we're at it
    result = pow(result, vec3(1.0 / gamma));
    color = vec4(result, 1.0);
}