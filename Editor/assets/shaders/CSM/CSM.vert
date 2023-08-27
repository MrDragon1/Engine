#version 460 core

#extension GL_KHR_vulkan_glsl : enable
#extension GL_GOOGLE_include_directive : enable

layout(location = 0) in vec3 a_Position;

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
void main()
{
    gl_Position = u_RenderPrimitive.ModelMatrix * vec4(a_Position, 1.0);
}