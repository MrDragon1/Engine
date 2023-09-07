#version 460 core

#extension GL_KHR_vulkan_glsl : enable
#extension GL_GOOGLE_include_directive : enable

layout(location = 0) in vec3 a_Position;
#include "Common.glslh"
void main()
{
    gl_Position = u_RenderPrimitive.ModelMatrix * vec4(a_Position, 1.0);
}