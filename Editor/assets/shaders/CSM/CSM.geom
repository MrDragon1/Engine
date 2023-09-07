#version 460 core

#extension GL_KHR_vulkan_glsl : enable
#extension GL_GOOGLE_include_directive : enable

layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

#include "Common.glslh"

void main()
{
    for (int i = 0; i < 3; ++i)
    {
        gl_Position = u_Shadow.DirLightMatrices[gl_InvocationID] * gl_in[i].gl_Position;
        gl_Layer = gl_InvocationID;
        EmitVertex();
    }
    EndPrimitive();
}