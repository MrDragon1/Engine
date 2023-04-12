#version 460 core
#extension GL_GOOGLE_include_directive : enable
#include "Common.glslh"

layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

uniform sampler2D u_AlbedoMap;
uniform sampler2D u_NormalMap;
uniform sampler2D u_MetallicMap;
uniform sampler2D u_RoughnessMap;
uniform sampler2D u_OcclusionMap;

// IBL
uniform samplerCube u_IrradianceMap;
uniform samplerCube u_PrefilterMap;
uniform sampler2D u_BRDFLUT;

uniform sampler2DArray u_ShadowMap;


const float PI = 3.14159265359;

// Used in PBR shader
struct PBRParameters
{
    vec3 Albedo;
    float Roughness;
    float Metalness;

    vec3 Normal;
    vec3 View;
    float NdotV;
} m_Params;

// ----------------------------------------------------------------------------
// Easy trick to get tangent-normals to world-space to keep PBR code simplified.
// Don't worry if you don't get what's going on; you generally want to do normal
// mapping the usual way for performance anways; I do plan make a note of this
// technique somewhere later in the normal mapping tutorial.
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(u_NormalMap, v_TexCoord).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(v_WorldPos);
    vec3 Q2  = dFdy(v_WorldPos);
    vec2 st1 = dFdx(v_TexCoord);
    vec2 st2 = dFdy(v_TexCoord);

    vec3 N   = normalize(v_Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}
// GGX/Towbridge-Reitz normal distribution function.
// Uses Disney's reparametrization of alpha = roughness^2
float NdfGGX(float cosLh, float roughness)
{
    float alpha = roughness * roughness;
    float alphaSq = alpha * alpha;

    float denom = (cosLh * cosLh) * (alphaSq - 1.0) + 1.0;
    return alphaSq / (PI * denom * denom);
}

// Single term for separable Schlick-GGX below.
float GaSchlickG1(float cosTheta, float k)
{
    return cosTheta / (cosTheta * (1.0 - k) + k);
}

// Schlick-GGX approximation of geometric attenuation function using Smith's method.
float GaSchlickGGX(float cosLi, float NdotV, float roughness)
{
    float r = roughness + 1.0;
    float k = (r * r) / 8.0; // Epic suggests using this roughness remapping for analytic lights.
    return GaSchlickG1(cosLi, k) * GaSchlickG1(NdotV, k);
}

float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r * r) / 8.0;

    float nom = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}

float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}

// Shlick's approximation of the Fresnel factor.
vec3 FresnelSchlick(vec3 F0, float cosTheta)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}

vec3 FresnelSchlickRoughness(vec3 F0, float cosTheta, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

// ----------------------------------------------------------------------------
// Cascaded Shadow Map
float ShadowCalculation(vec3 fragPosWorldSpace)
{
    // select cascade layer
    vec4 fragPosViewSpace = u_View.ViewMatrix * vec4(fragPosWorldSpace, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    int layer = -1;
    for (int i = 0; i < u_Shadow.CascadeCount; ++i)
    {
        if (depthValue < u_Shadow.CascadeSplits[i].x)
        {
            layer = i;
            break;
        }
    }
    if (layer == -1)
    {
        layer = u_Shadow.CascadeCount;
    }


    vec4 fragPosLightSpace = u_Shadow.DirLightMatrices[layer] * vec4(fragPosWorldSpace, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    float bias = max(0.001 * (1.0 - dot(m_Params.Normal, u_Light.Direction)), 0.000);
    float shadowMapDepth = texture(u_ShadowMap, vec3(projCoords.xy * 0.5 + 0.5, layer)).r;

    return step(projCoords.z * 0.5 + 0.5, shadowMapDepth + bias);

    //    // get depth of current fragment from light's perspective
    //    float currentDepth = projCoords.z;
    //
    //    // keep the shadow at 0.0 when outside the far_plane region of the light's frustum.
    //    if (currentDepth > 1.0)
    //    {
    //        return 0.0;
    //    }
    //    // calculate bias (based on depth map resolution and slope)
    //    vec3 normal = u_UseNormalMap ? getNormalFromMap() : normalize(v_Normal);
    //    float bias = max(0.05 * (1.0 - dot(normal, u_Light.Direction)), 0.005);
    //    const float biasModifier = 0.5f;
    //    if (layer == u_Shadow.CascadeCount)
    //    {
    //        bias *= 1 / (u_View.FarPlane * biasModifier);
    //    }
    //    else
    //    {
    //        bias *= 1 / ( u_Shadow.CascadeSplits[layer] * biasModifier);
    //    }
    //
    //    // PCF
    //    float shadow = 0.0;
    //    vec2 texelSize = 1.0 / vec2(textureSize(u_ShadowMap, 0));
    //    for (int x = -1; x <= 1; ++x)
    //    {
    //        for (int y = -1; y <= 1; ++y)
    //        {
    //            float pcfDepth = texture(u_ShadowMap, vec3(projCoords.xy + vec2(x, y) * texelSize, layer)).r;
    //            shadow += (currentDepth - bias) > pcfDepth ? 1.0 : 0.0;
    //        }
    //    }
    //    shadow /= 9.0;
    //
    //    return shadow;
}

vec3 IBL(vec3 F0, vec3 Lr)
{
    vec3 irradiance = texture(u_IrradianceMap, m_Params.Normal).rgb;
    vec3 F = FresnelSchlickRoughness(F0, m_Params.NdotV, m_Params.Roughness);
    vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
    vec3 diffuseIBL = m_Params.Albedo * irradiance;

    int envRadianceTexLevels = textureQueryLevels(u_IrradianceMap);
    float NoV = clamp(m_Params.NdotV, 0.0, 1.0);
    vec3 R = 2.0 * dot(m_Params.View, m_Params.Normal) * m_Params.Normal - m_Params.View;
    vec3 specularIrradiance = textureLod(u_PrefilterMap, R, (m_Params.Roughness) * envRadianceTexLevels).rgb;
    //specularIrradiance = vec3(Convert_sRGB_FromLinear(specularIrradiance.r), Convert_sRGB_FromLinear(specularIrradiance.g), Convert_sRGB_FromLinear(specularIrradiance.b));

    // Sample BRDF Lut, 1.0 - roughness for y-coord because texture was generated (in Sparky) for gloss model
    vec2 specularBRDF = texture(u_BRDFLUT, vec2(m_Params.NdotV, 1.0 - m_Params.Roughness)).rg;
    vec3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

    return kd * diffuseIBL + specularIBL;
}

vec3 CalculateDirLights(vec3 F0)
{
    vec3 result = vec3(0.0);
    for (int i = 0; i < 1; i++) //Only one light for now
    {
        vec3 Li = u_Light.Direction;
        //        vec3 Lradiance = u_Light.Radiance * u_Light.Multiplier;
        vec3 Lradiance = vec3(1.0f);
        vec3 Lh = normalize(Li + m_Params.View);

        // Calculate angles between surface normal and various light vectors.
        float cosLi = max(0.0, dot(m_Params.Normal, Li));
        float cosLh = max(0.0, dot(m_Params.Normal, Lh));

        vec3 F = FresnelSchlickRoughness(F0, max(0.0, dot(Lh, m_Params.View)), m_Params.Roughness);
        float D = NdfGGX(cosLh, m_Params.Roughness);
        float G = GaSchlickGGX(cosLi, m_Params.NdotV, m_Params.Roughness);

        vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
        vec3 diffuseBRDF = kd * m_Params.Albedo;

        // Cook-Torrance
        vec3 specularBRDF = (F * D * G) / max(0.00001, 4.0 * cosLi * m_Params.NdotV);
        specularBRDF = clamp(specularBRDF, vec3(0.0f), vec3(10.0f));
        result += (diffuseBRDF + specularBRDF) * Lradiance * cosLi;
    }
    return result;
}

// ----------------------------------------------------------------------------
void main()
{
    m_Params.Albedo     = (u_RenderPrimitive.UseMap & 1<<1) == 0 ? pow(texture(u_AlbedoMap, v_TexCoord).rgb, vec3(2.2)) : u_RenderPrimitive.Albedo.rgb;
    m_Params.Metalness  = (u_RenderPrimitive.UseMap & 1<<3) == 0 ? texture(u_MetallicMap, v_TexCoord).r : u_RenderPrimitive.Metallic;
    m_Params.Roughness = (u_RenderPrimitive.UseMap & 1<<4) == 0 ? texture(u_RoughnessMap, v_TexCoord).r : u_RenderPrimitive.Roughness;
    float ao        = (u_RenderPrimitive.UseMap & 1<<5) == 0 ? texture(u_OcclusionMap, v_TexCoord).r : u_RenderPrimitive.Occlusion;

    m_Params.Normal = (u_RenderPrimitive.UseMap & 1<<2) == 0 ? getNormalFromMap() : normalize(v_Normal);
    m_Params.View = normalize(u_View.CameraPosition - v_WorldPos);
    vec3 R = reflect(-m_Params.View, m_Params.Normal);
    m_Params.NdotV = max(dot(m_Params.Normal, m_Params.View), 0.0);
    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)
    // Specular reflection vector
    vec3 Lr = 2.0 * m_Params.NdotV * m_Params.Normal - m_Params.View;
    // Fresnel reflectance, metals use albedo
    vec3 F0 = mix(vec3(0.04), m_Params.Albedo, m_Params.Metalness);

    float shadowScale = ShadowCalculation(v_WorldPos);
    shadowScale = 1.0 - clamp(1.0f - shadowScale, 0.0f, 1.0f);

    vec3 lightContribution = CalculateDirLights(F0) * shadowScale;
    lightContribution += m_Params.Albedo * u_RenderPrimitive.Emisstion;
    vec3 iblContribution = IBL(F0, Lr) * 1.0f;

    vec4 color = vec4(iblContribution + lightContribution , 1.0);

    FragColor = color;

    EntityID = u_View.EntityID;
}
