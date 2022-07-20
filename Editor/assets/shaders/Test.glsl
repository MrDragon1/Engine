// Basic Texture Shader

#type vertex
#version 460 core

layout(location = 0) in vec3 a_Position;
layout(location = 1) in vec3 a_Normal;
layout(location = 2) in vec4 a_Color;
layout(location = 3) in vec2 a_TexCoord;
layout(location = 4) in int a_EntityID;

uniform mat4 u_ViewProjection;
uniform mat4 u_Model;
uniform mat4 u_LightSpaceMatrix;

out flat int v_EntityID;
out VS_OUT {
    vec4 Color;
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} vs_out;

void main()
{
    v_EntityID = a_EntityID;
    gl_Position =  u_ViewProjection * u_Model * vec4(a_Position, 1.0);
    vs_out.FragPos = vec3(u_Model * vec4(a_Position, 1.0));
    vs_out.Normal = transpose(inverse(mat3(u_Model))) * a_Normal;
    vs_out.TexCoords = a_TexCoord;
    vs_out.FragPosLightSpace = u_LightSpaceMatrix * vec4(vs_out.FragPos, 1.0);
    vs_out.Color = a_Color;
}

#type fragment
#version 460 core

layout(location = 0) out vec4 color;
layout(location = 1) out int color2;

in flat int v_EntityID;
in VS_OUT {
    vec4 Color;
    vec3 FragPos;
    vec3 Normal;
    vec2 TexCoords;
    vec4 FragPosLightSpace;
} fs_in;

uniform sampler2D u_BaseColorTexture;
uniform sampler2D u_MetallicTexture;
uniform sampler2D u_NormalTexture;
uniform sampler2D u_OcclusionTexture;
uniform sampler2D u_EmissionTexture;
uniform sampler2D u_ShadowMap;

float near = 0.1;
float far  = 7.5;

float LinearizeDepth(float depth) 
{
    float z = depth * 2.0 - 1.0; // back to NDC 
    return (2.0 * near * far) / (far + near - z * (far - near));
}

float ShadowCalculation(vec4 fragPosLightSpace)
{
    // 执行透视除法
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // 变换到[0,1]的范围
    projCoords = projCoords * 0.5 + 0.5;
    // 取得最近点的深度(使用[0,1]范围下的fragPosLight当坐标)
    float closestDepth = texture(u_ShadowMap, projCoords.xy).r; 
    // 取得当前片段在光源视角下的深度
    float currentDepth = projCoords.z;
    // 检查当前片段是否在阴影中
    float shadow = currentDepth > closestDepth + 0.005  ? 1.0 : 0;

    return shadow;
}
void main()
{
    float shadow = ShadowCalculation(fs_in.FragPosLightSpace);
    color =  texture(u_BaseColorTexture, fs_in.TexCoords) * (1-shadow);
    color = vec4(color.xyz,1.0);
    color2 = v_EntityID;
}