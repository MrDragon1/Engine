#version 460 core

#extension GL_GOOGLE_include_directive : enable
#include "Common.glslh"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;


layout(location = 0) out vec3 v_WorldPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;

void main()
{
    v_TexCoord = a_TexCoord;
    v_WorldPos = vec3(u_RenderPrimitive.ModelMatrix * vec4(a_Position, 1.0));;
    v_Normal = mat3(u_RenderPrimitive.ModelMatrix) * a_Normal;
    gl_Position = u_View.ViewProjectionMatrix * vec4(v_WorldPos, 1.0);
}
