#version 460 core
layout(location = 0) in vec3 a_Position;
#extension GL_GOOGLE_include_directive : enable
#include "Common.glslh"

void main()
{
    gl_Position = u_RenderPrimitive.ModelMatrix * vec4(a_Position, 1.0);
}