#version 460 core

#extension GL_GOOGLE_include_directive : enable
#include "common.glslh"

layout(location = 0) in vec3 a_Position;

layout(location = 0) out vec3 v_LocalPos;

void main()
{
    v_LocalPos = a_Position;

    mat4 rotView = mat4(mat3(u_Camera.ViewMatrix)); // remove translation from the view matrix
    vec4 clipPos = u_Camera.ProjectionMatrix * rotView * vec4(v_LocalPos, 1.0);
    gl_Position = clipPos.xyww;
}