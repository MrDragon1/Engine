#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec4 v_LocalPos;
layout(location = 1) in vec2 v_TexCoord;

layout(binding=0) uniform sampler2D tex;

void main()
{
    FragColor = vec4(texture(tex,v_TexCoord).rgba);
}