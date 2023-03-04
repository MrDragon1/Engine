#version 460 core

layout(location = 0) in vec4 a_Position;

layout(location = 0) out vec4 v_LocalPos;

void main()
{
    v_LocalPos = a_Position;
    gl_Position = a_Position;
}