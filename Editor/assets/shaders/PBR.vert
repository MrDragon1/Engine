#version 460 core
#extension GL_KHR_vulkan_glsl : enable
#extension GL_GOOGLE_include_directive : enable
layout(std140, set=0, binding = 0) uniform View
{
    vec3 CameraPosition;
    float EnvironmentMapIntensity;
    vec4 ScissorNormalized;

    mat4 ViewProjectionMatrix;
    mat4 InverseViewProjectionMatrix;
    mat4 ProjectionMatrix;
    mat4 InverseProjectionMatrix;
    mat4 ViewMatrix;
    mat4 InverseViewMatrix;
    float FarPlane;
    float NearPlane;

    int EntityID;
    float padding;

    // Fog 
    vec4  FogColor;
    float FogStart;
    float FogDensity;
    float FogHeight;
    float FogHeightFalloff;
    float FogScatteringStart;
    float FogScatteringSize;
    bool  FogEnable;
    bool  FogFromIBL;
} u_View;

layout(std140, set=0, binding = 1) uniform Shadow
{
    vec4 CascadeSplits[16];
    mat4 DirLightMatrices[16];
    int CascadeCount;
} u_Shadow;

layout(std140, set=0, binding = 2) uniform Light
{
    vec3 Direction;
    float ShadowAmount;
    vec3 Radiance;
    float Multiplier;
} u_Light;


layout(std140, set=0, binding = 3) uniform RenderPrimitive
{
    mat4 ModelMatrix;

    vec4 Albedo;
    float Metallic;
    float Roughness;
    float Occlusion;
    float Emisstion;
    int UseMap; // 1<<1: Albedo 1<<2: Normal 1<<3: Metallic 1<<4: Roughness 1<<5: Occlusion
    
} u_RenderPrimitive;

layout(std140, set=0, binding = 4) uniform RenderPrimitiveBone
{
    mat4 BoneTransform[100];
} u_RenderPrimitiveBone;
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
