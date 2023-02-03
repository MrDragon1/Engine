#version 460 core
#extension GL_GOOGLE_include_directive : enable
#include "Common.glslh"

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec3 a_Tangent;
layout(location = 3) in vec3 a_Binormal;
layout(location = 4) in vec2 a_TexCoord;

layout(location = 5) in ivec4 a_BoneIDs;
layout(location = 6) in vec4 a_BoneWeights;

layout(location = 0) uniform mat4 u_Model;

const int MAX_BONES = 100;
layout(location = 1) uniform mat4 u_BoneTransforms[100];

layout(location = 0) out vec3 v_WorldPos;
layout(location = 1) out vec3 v_Normal;
layout(location = 2) out vec2 v_TexCoord;

void main()
{
    mat4 boneTransform = u_BoneTransforms[a_BoneIDs[0]] * a_BoneWeights[0];
    boneTransform += u_BoneTransforms[a_BoneIDs[1]] * a_BoneWeights[1];
    boneTransform += u_BoneTransforms[a_BoneIDs[2]] * a_BoneWeights[2];
    boneTransform += u_BoneTransforms[a_BoneIDs[3]] * a_BoneWeights[3];


    v_TexCoord = a_TexCoord;
    v_WorldPos = vec3(u_Model * boneTransform * vec4(a_Position, 1.0));;
    v_Normal = mat3(u_Model) * mat3(boneTransform) * a_Normal;
    gl_Position = u_Camera.ViewProjectionMatrix * vec4(v_WorldPos, 1.0);
}
