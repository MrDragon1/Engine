#version 460 core

layout(location = 0) out vec4 color;
layout(location = 0) in vec2 v_TexCoord;

layout(std140, binding = 0) uniform Config{
    float Threshold;
    float Knee;
    float Intensity;
    int DownSample;
    int MipLevel;
}u_Config;

layout(binding = 0) uniform sampler2D u_MainImage;

void main()
{
    vec3 result = texture(u_MainImage, v_TexCoord).rgb;
    float brightness = dot(result, vec3(0.2126, 0.7152, 0.0722));
    if (brightness > u_Config.Threshold) color = vec4(result, 1.0);
    else color = vec4(0.0, 0.0, 0.0, 1.0);
}