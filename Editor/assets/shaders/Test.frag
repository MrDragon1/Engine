#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec4 v_LocalPos;

void main()
{
    FragColor = vec4(v_LocalPos.xyz, 1.0);
}