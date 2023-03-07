#version 460 core

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec2 a_TexCoord;

layout(location = 0) out vec4 v_LocalPos;
layout(location = 1) out vec2 v_TexCoord;

void main()
{
    v_LocalPos = a_Position;
    v_TexCoord = a_TexCoord;
    gl_Position = a_Position;
}