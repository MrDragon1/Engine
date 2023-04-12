#version 460 core
layout(triangles, invocations = 5) in;
layout(triangle_strip, max_vertices = 3) out;

#extension GL_GOOGLE_include_directive : enable
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