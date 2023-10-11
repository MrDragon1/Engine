#version 460 core
#extension GL_GOOGLE_include_directive : enable
#include "Common.glslh"
#include "PBR.glslh"
#include "ShadowMapping.glslh"


layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

layout(set=1, binding = 10) uniform sampler2D u_AlbedoMap;
layout(set=1, binding = 11) uniform sampler2D u_NormalMap;
layout(set=1, binding = 12) uniform sampler2D u_MetallicMap;
layout(set=1, binding = 13) uniform sampler2D u_RoughnessMap;
layout(set=1, binding = 14) uniform sampler2D u_OcclusionMap;
              
// IBL        
layout(set=1, binding = 15) uniform sampler2D u_BRDFLUT;
layout(set=1, binding = 17) uniform samplerCube u_IrradianceMap;
layout(set=1, binding = 16) uniform samplerCube u_PrefilterMap;
              
layout(set=1, binding = 18) uniform sampler2DArray u_ShadowMap;


// Used in PBR shader
struct PBRParameters
{
    vec3 Albedo;
    float Roughness;
    float Metalness;

    vec3 Normal;
    vec3 View;
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


vec4 fog(vec4 color, vec3 view) {
    if (u_View.FogDensity > 0.0) {
        float A = u_View.FogDensity;
        float B = u_View.FogHeightFalloff;

        float d = length(view);

        float h = max(0.001, view.y);
        // The function below is continuous at h=0, so to avoid a divide-by-zero, we just clamp h
        float fogIntegralFunctionOfDistance = A * ((1.0 - exp(-B * h)) / h);
        float fogIntegral = fogIntegralFunctionOfDistance * max(d - u_View.FogStart, 0.0);
        float fogOpacity = max(1.0 - exp2(-fogIntegral), 0.0);

        // don't go above requested max opacity
        fogOpacity = min(fogOpacity, 100.0f); //frameUniforms.fogMaxOpacity

        // compute fog color
        vec3 fogColor = u_View.FogColor.rgb;

//        if (u_View.FogFromIBL > 0.0) {
//            // get fog color from envmap
//            float lod = 1.0f;
//            fogColor *= textureLod(light_iblSpecular, view, lod).rgb * 1.0f;
//        }

        fogColor *= fogOpacity;
        if (u_View.FogScatteringSize > 0.0) {
            // compute a new line-integral for a different start distance
            float inscatteringIntegral = fogIntegralFunctionOfDistance *
            max(d - u_View.FogScatteringStart, 0.0);
            float inscatteringOpacity = max(1.0 - exp2(-inscatteringIntegral), 0.0);

            // Add sun colored fog when looking towards the sun
            vec3 sunColor = vec3(203.0f, 119.0f, 49.0f)/255.0f; //frameUniforms.lightColorIntensity.rgb * frameUniforms.lightColorIntensity.w;
            float sunAmount = max(dot(view, u_Light.Direction) / d, 0.0); // between 0 and 1
            float sunInscattering = pow(sunAmount, u_View.FogScatteringSize);

            fogColor += sunColor * (sunInscattering * inscatteringOpacity);
        }

        color.rgb = color.rgb * (1.0 - fogOpacity) + fogColor;
    }
    return color;
}



/**
 * Returns the cascade index for this fragment (between 0 and CONFIG_MAX_SHADOW_CASCADES - 1).
 */
uint getShadowCascade() {
    // select cascade layer
    vec4 fragPosViewSpace = u_View.ViewMatrix * vec4(v_WorldPos, 1.0);
    float depthValue = abs(fragPosViewSpace.z);

    uint layer = -1;
    for (uint i = 0; i < u_Shadow.CascadeCount; ++i)
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

    return layer;
}

highp vec4 getCascadeLightSpacePosition(uint cascade) {

    highp vec3 p = v_WorldPos;
    highp float cosTheta = saturate(dot(m_Params.Normal, u_Light.Direction));
    highp float sinTheta = sqrt(1.0 - cosTheta * cosTheta);
    p += m_Params.Normal * (sinTheta * 0.02);

    vec4 fragPosLightSpace = u_Shadow.DirLightMatrices[cascade] * vec4(p, 1.0);
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;

    return fragPosLightSpace;
}

float ShadowCalculation()
{
    uint layer = getShadowCascade();

    vec4 fragPosLightSpace = getCascadeLightSpacePosition(layer);
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

// Shadow requiring a sampler2D sampler (VSM, DPCF and PCSS)
float shadow(const bool DIRECTIONAL,
const mediump sampler2DArray shadowMap,
const uint index, highp vec4 shadowPosition, highp float zLight) {
    highp vec4 scissorNormalized = u_View.ScissorNormalized;
    uint layer = index;

    return ShadowSample_PCSS(DIRECTIONAL, shadowMap, scissorNormalized, layer, index,
        shadowPosition, zLight);

}

vec3 isotropicLobe(const vec3 h,
float NoV, float NoL, float NoH, float LoH) {
    vec3 F0 = mix(vec3(0.04), m_Params.Albedo, m_Params.Metalness);// TDOO: for temp

    float D = distribution(m_Params.Roughness, NoH, h);
    float V = visibility(m_Params.Roughness, NoV, NoL);
    vec3  F = fresnel(F0, LoH);

    return (D * V) * F;
}

vec3 specularLobe(const vec3 h, float NoV, float NoL, float NoH, float LoH) {
    return isotropicLobe(h, NoV, NoL, NoH, LoH);
}

vec3 diffuseLobe(float NoV, float NoL, float LoH) {
    return m_Params.Albedo * diffuse(m_Params.Roughness, NoV, NoL, LoH);
}

vec3 surfaceShading(float occlusion) {
    vec3 h = normalize(m_Params.View + u_Light.Direction);

    float NoV = max(dot(m_Params.Normal, m_Params.View),1e-4);
    float NoL = saturate(dot(m_Params.Normal, u_Light.Direction));
    float NoH = saturate(dot(m_Params.Normal, h));
    float LoH = saturate(dot(u_Light.Direction, h));

    vec3 Fr = specularLobe(h, NoV, NoL, NoH, LoH);
    vec3 Fd = diffuseLobe(NoV, NoL, LoH);

    // The energy compensation term is used to counteract the darkening effect
    // at high roughness
    vec3 color = Fd + Fr; // * pixel.energyCompensation

    return (color * u_Light.Radiance) * (NoL * occlusion);
}

void evaluateDirectionalLight(inout vec3 color) {
    float visibility = 1.0;
    uint cascade = getShadowCascade();
    highp vec4 shadowPosition = getCascadeLightSpacePosition(cascade);
    visibility = 1.0 - shadow(true, u_ShadowMap, cascade, shadowPosition, 0.0f);

    if (visibility <= 0.0) {
        return;
    }
    //Wierd bug, if I don't do this, the shadow is not correct
    color.rgb += surfaceShading(visibility);
}

void evaluateIBL(inout vec3 color) {
    float NoV = dot(m_Params.Normal, m_Params.View);

    vec3 Lr = 2.0 * NoV * m_Params.Normal - m_Params.View;
    vec3 F0 = mix(vec3(0.04), m_Params.Albedo, m_Params.Metalness);

    vec3 irradiance = texture(u_IrradianceMap, m_Params.Normal).rgb;
    vec3 F = F_Schlick(F0, m_Params.Roughness, NoV);
    vec3 kd = (1.0 - F) * (1.0 - m_Params.Metalness);
    vec3 diffuseIBL = m_Params.Albedo * irradiance;

    int envRadianceTexLevels = textureQueryLevels(u_IrradianceMap);
    NoV = clamp(NoV, 0.0, 1.0);
    vec3 R = 2.0 * dot(m_Params.View, m_Params.Normal) * m_Params.Normal - m_Params.View;
    vec3 specularIrradiance = textureLod(u_PrefilterMap, R, (m_Params.Roughness) * envRadianceTexLevels).rgb;
    //specularIrradiance = vec3(Convert_sRGB_FromLinear(specularIrradiance.r), Convert_sRGB_FromLinear(specularIrradiance.g), Convert_sRGB_FromLinear(specularIrradiance.b));

    // Sample BRDF Lut, 1.0 - roughness for y-coord because texture was generated (in Sparky) for gloss model
    vec2 specularBRDF = texture(u_BRDFLUT, vec2(NoV, 1.0 - m_Params.Roughness)).rg;
    vec3 specularIBL = specularIrradiance * (F0 * specularBRDF.x + specularBRDF.y);

    color += kd * diffuseIBL + specularIBL;
}

vec4 evaluateLights() {
    // Ideally we would keep the diffuse and specular components separate
    // until the very end but it costs more ALUs on mobile. The gains are
    // currently not worth the extra operations
    vec3 color = vec3(0.0);

    // We always evaluate the IBL as not having one is going to be uncommon,
    // it also saves 1 shader variant
    evaluateIBL(color);

    evaluateDirectionalLight(color);

    // In fade mode we un-premultiply baseColor early on, so we need to
    // premultiply again at the end (affects diffuse and specular lighting)
//    color *= u_Params.Albedo.a;

    return vec4(color, 1.0); // computeDiffuseAlpha(u_Params.Albedo.a)
}

vec4 evaluateMaterial() {
    vec4 color = evaluateLights();
    return color;
}


// ----------------------------------------------------------------------------
void main()
{
    m_Params.Albedo     = (u_RenderPrimitive.UseMap & 1<<1) != 0 ? pow(texture(u_AlbedoMap, v_TexCoord).rgb, vec3(2.2)) : u_RenderPrimitive.Albedo.rgb;
    m_Params.Metalness  = (u_RenderPrimitive.UseMap & 1<<3) != 0 ? texture(u_MetallicMap, v_TexCoord).r : u_RenderPrimitive.Metallic;
    m_Params.Roughness = (u_RenderPrimitive.UseMap & 1<<4) != 0 ? texture(u_RoughnessMap, v_TexCoord).r : u_RenderPrimitive.Roughness;

    m_Params.Normal = (u_RenderPrimitive.UseMap & 1<<2) != 0 ? getNormalFromMap() : normalize(v_Normal);
    m_Params.Normal = gl_FrontFacing ? m_Params.Normal : -m_Params.Normal; // for two-sided materials
    m_Params.View = normalize(u_View.CameraPosition - v_WorldPos);

    FragColor = evaluateMaterial();

    if(u_View.FogEnable) FragColor = fog(FragColor, v_WorldPos - u_View.CameraPosition);

    EntityID = u_RenderPrimitive.EntityID;
}
