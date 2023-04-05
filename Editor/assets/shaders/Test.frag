#version 460 core

layout(location = 0) out vec4 FragColor;
layout(location = 0) in vec4 v_LocalPos;
layout(location = 1) in vec2 v_TexCoord;

layout(binding=0) uniform sampler2D tex;
layout (std140, binding = 1) uniform Data {
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
    // FragColor = vec4(texture(tex,v_TexCoord).rgba);
    FragColor = vec4(uData.FogColor.xyz,1.0);
}