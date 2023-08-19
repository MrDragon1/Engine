#version 460 core
#extension GL_KHR_vulkan_glsl : enable
layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;
layout(location = 0) in vec4 v_LocalPos;
layout(location = 1) in vec2 v_TexCoord;

layout(set=1, binding=17) uniform samplerCube tex;
layout (std140, set=0, binding = 1) uniform Data {
    vec3 CameraPosition;
    float EnvironmentMapIntensity;
    vec4 ScissorNormalized;

    // Fog
    vec4 FogColor;
    float FogStart;
    float FogDensity;
    float FogHeight;
    float FogHeightFalloff;
    float FogScatteringStart;
    float FogScatteringSize;
    bool FogEnable;
    bool FogFromIBL;
}uData;

void main()
{
    FragColor = vec4(texture(tex,vec3(v_TexCoord,0.0)).rgba);
    // FragColor = vec4(v_LocalPos.xyz,1.0);
    EntityID = 2;
}