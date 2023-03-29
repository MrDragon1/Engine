#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec4 v_LocalPos;
layout(location = 1) in vec2 v_TexCoord;

layout(binding=0) uniform sampler2D tex;
layout (std140, binding = 1) uniform Data {
    vec4 f4a;
    vec4 f4b;
    float f1a;
    vec2 f2a;
}uData;
void main()
{
    // FragColor = vec4(texture(tex,v_TexCoord).rgba);
    FragColor = vec4(uData.f4a.xyz,1.0);
}