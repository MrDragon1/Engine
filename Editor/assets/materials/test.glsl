#version 460

struct BSDF { vec3 response; vec3 throughput; float thickness; float ior; };
struct surfaceshader { vec3 color; vec3 transparency; };
#define material surfaceshader

uniform vec3 SR_marble1_L;
uniform float SR_marble1_clearcoat;
uniform float SR_marble1_roughness;
uniform vec3 SR_marble1_V;
uniform float SR_marble1_specularTint;
uniform vec3 SR_marble1_baseColor;
uniform float SR_marble1_metallic;
uniform float SR_marble1_specular;
uniform float SR_marble1_subsurface;
uniform float SR_marble1_anisotropic;
uniform float SR_marble1_sheen;
uniform float SR_marble1_sheenTint;
uniform float SR_marble1_clearcoatGloss;

in vertexData
{
    vec3 positionWorld;
    vec3 normalWorld;
    vec3 tangentWorld;
    vec3 bitangentWorld;
} vd;

// Pixel Shader Output
layout(location = 0) out vec4 out1;

#define PI 3.1415926535897932
#define PI_INV (1.0 / PI)

float sqr(float x) { return x*x; }

float SchlickFresnel(float u)
{
    float m = clamp(1-u, 0, 1);
    float m2 = m*m;
    return m2*m2*m; // pow(m,5)
}

float GTR1(float NdotH, float a)
{
    if (a >= 1) return 1/PI;
    float a2 = a*a;
    float t = 1 + (a2-1)*NdotH*NdotH;
    return (a2-1) / (PI*log(a2)*t);
}

float GTR2(float NdotH, float a)
{
    float a2 = a*a;
    float t = 1 + (a2-1)*NdotH*NdotH;
    return a2 / (PI * t*t);
}

float GTR2_aniso(float NdotH, float HdotX, float HdotY, float ax, float ay)
{
    return 1 / (PI * ax*ay * sqr( sqr(HdotX/ax) + sqr(HdotY/ay) + NdotH*NdotH ));
}

float smithG_GGX(float NdotV, float alphaG)
{
    float a = alphaG*alphaG;
    float b = NdotV*NdotV;
    return 1 / (NdotV + sqrt(a + b - a*b));
}

float smithG_GGX_aniso(float NdotV, float VdotX, float VdotY, float ax, float ay)
{
    return 1 / (NdotV + sqrt( sqr(VdotX*ax) + sqr(VdotY*ay) + sqr(NdotV) ));
}
void disney_clearcoat(float clearcoat, float clearcoatGloss, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float NdotH = dot(N,H);
    float LdotH = dot(L,H);
    float NdotL = dot(N,L);
    float NdotV = dot(N,V);
    float FH = SchlickFresnel(LdotH);

    float Dr = GTR1(NdotH, mix(.1,.001,clearcoatGloss));
    float Fr = mix(.04, 1.0, FH);
    float Gr = smithG_GGX(NdotL, .25) * smithG_GGX(NdotV, .25);
    out1.response = vec3(clearcoat * Gr * Fr * Dr);
}

void mx_luminance_color3(vec3 _in, vec3 lumacoeffs, out vec3 result)
{
    result = vec3(dot(_in, lumacoeffs));
}

void HanrahanKrueger_subsurface(vec3 color, float roughness, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = dot(L,H);
    float NdotL = dot(N,L);
    float NdotV = dot(N,V);
    float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
    float Fss90 = LdotH * LdotH * roughness;
    float Fss = mix(1.0, Fss90, FL) * mix(1.0, Fss90, FV);
    float ss = 1.25 * (Fss * (1 / (NdotL + NdotV) - .5) + .5);
    out1.response = ss * color * PI_INV;
}

void disney_diffuse(vec3 color, float roughness, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = dot(L,H);
    float NdotL = dot(N,L);
    float NdotV = dot(N,V);

    float FL = SchlickFresnel(NdotL), FV = SchlickFresnel(NdotV);
    float Fd90 = 0.5 + 2 * LdotH * LdotH * roughness;
    float Fd = mix(1.0, Fd90, FL) * mix(1.0, Fd90, FV);
    out1.response = Fd * color * PI_INV;
}

void disney_sheen(vec3 color, float sheen, float sheenTint, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = dot(L,H);
    float FH = SchlickFresnel(LdotH);
    vec3 Csheen = mix(vec3(1), color, sheenTint);
    out1.response = FH * sheen * Csheen;
}

void disney_sheen(vec3 color, float sheen, float sheenTint, vec3 L, vec3 V, vec3 N, inout BSDF out1)
{
    vec3 H = normalize(L+V);
    float LdotH = dot(L,H);
    float FH = SchlickFresnel(LdotH);
    vec3 Csheen = mix(vec3(1), color, sheenTint);
    out1.response = FH * sheen * Csheen;
}

void surface(BSDF bsdf, float opacity, inout surfaceshader result)
{
    result.color = bsdf.response;
    result.transparency = vec3(opacity);
}

void NG_disney_bsdf_2012_surface(vec3 L, vec3 V, vec3 N, vec3 X, vec3 Y, vec3 baseColor, float metallic, float subsurface, float specular, float roughness, float specularTint, float anisotropic, float sheen, float sheenTint, float clearcoat, float clearcoatGloss, out surfaceshader out1)
{
    vec3 Cdlin_out1 = pow(baseColor, vec3(float(2.200000)));
    BSDF clearcoat_bsdf_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    disney_clearcoat(clearcoat, clearcoatGloss, L, V, N, clearcoat_bsdf_out1);
    float one_minus_metallic_out1 = float(1.000000) - metallic;
    vec3 Cdlum_out1 = vec3(1.0);
    mx_luminance_color3(Cdlin_out1, vec3(0.272229, 0.674082, 0.053689), Cdlum_out1);
    BSDF subsurface_bsdf_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    HanrahanKrueger_subsurface(Cdlin_out1, roughness, L, V, N, subsurface_bsdf_out1);
    BSDF diffuse_bsdf_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    disney_diffuse(Cdlin_out1, roughness, L, V, N, diffuse_bsdf_out1);
    float quater_clearcoat_out1_in2_clamped = clamp(float(0.250000), 0.0, 1.0);
    BSDF quater_clearcoat_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    quater_clearcoat_out1.response = clearcoat_bsdf_out1.response * quater_clearcoat_out1_in2_clamped.response;
    quater_clearcoat_out1.throughput = clearcoat_bsdf_out1.throughput * quater_clearcoat_out1_in2_clamped.throughput;
    vec3 divide_C_out1 = Cdlin_out1 / Cdlum_out1;
    BSDF diffuse_mix_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    diffuse_mix_out1.response = mix(subsurface_bsdf_out1.response, diffuse_bsdf_out1.response, subsurface);
    diffuse_mix_out1.throughput = mix(subsurface_bsdf_out1.throughput, diffuse_bsdf_out1.throughput, subsurface);
    vec3 Ctint_out1 = (Cdlum_out1 > float(0.000000)) ? divide_C_out1 : Ctint_in2;
    vec3 spec_tint_mix_out1 = mix(Ctint_out1, vec3(1.000000, 1.000000, 1.000000), specularTint);
    BSDF sheen_bsdf_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    disney_sheen(Ctint_out1, sheen, sheenTint, L, V, N, sheen_bsdf_out1);
    vec3 spec_tint_tmp_out1 = spec_tint_mix_out1 * float(0.080000);
    BSDF sheen_add_out1_in2_clamped = clamp(sheen_bsdf_out1, 0.0, 1.0);
    BSDF sheen_add_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    sheen_add_out1.response = diffuse_mix_out1.response + sheen_bsdf_out1.response;
    sheen_add_out1.throughput = diffuse_mix_out1.throughput * sheen_bsdf_out1.throughput;
    vec3 spec_tint_tmp1_out1 = spec_tint_tmp_out1 * specular;
    float sheen_scale_out1_in2_clamped = clamp(one_minus_metallic_out1, 0.0, 1.0);
    BSDF sheen_scale_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    sheen_scale_out1.response = sheen_add_out1.response * sheen_scale_out1_in2_clamped.response;
    sheen_scale_out1.throughput = sheen_add_out1.throughput * sheen_scale_out1_in2_clamped.throughput;
    vec3 Cspec_out1 = mix(Cdlin_out1, spec_tint_tmp1_out1, metallic);
    BSDF specular_bsdf_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    disney_sheen(Cspec_out1, metallic, specular, roughness, specularTint, anisotropic, L, V, N, X, Y, specular_bsdf_out1);
    BSDF specular_add_out1_in2_clamped = clamp(specular_bsdf_out1, 0.0, 1.0);
    BSDF specular_add_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    specular_add_out1.response = sheen_scale_out1.response + specular_bsdf_out1.response;
    specular_add_out1.throughput = sheen_scale_out1.throughput * specular_bsdf_out1.throughput;
    BSDF clearcoat_add_out1_in2_clamped = clamp(quater_clearcoat_out1, 0.0, 1.0);
    BSDF clearcoat_add_out1 = BSDF(vec3(0.0),vec3(1.0), 0.0, 0.0);
    clearcoat_add_out1.response = specular_add_out1.response + quater_clearcoat_out1.response;
    clearcoat_add_out1.throughput = specular_add_out1.throughput * quater_clearcoat_out1.throughput;
    surfaceshader shader_constructor_out1 = surfaceshader(vec3(0.0),vec3(0.0));
    surface(clearcoat_add_out1, shader_constructor_opacity, shader_constructor_out1);
    out1 = shader_constructor_out1;
}

void surfacematerial(surfaceshader surfaceshader, out material result)
{
    result = surfaceshader;
}

void main()
{
    vec3 Iposition_out1 = vd.positionWorld;
    vec3 Inormal_out1 = vd.normalWorld;
    vec3 Itangent_out1 = vd.tangentWorld;
    vec3 Ibitangent_out1 = vd.tangentWorld;
    surfaceshader SR_marble1_out1 = surfaceshader(vec3(0.0),vec3(0.0));
    NG_disney_bsdf_2012_surface(SR_marble1_L, SR_marble1_V, Inormal_out1, Itangent_out1, Ibitangent_out1, SR_marble1_baseColor, SR_marble1_metallic, SR_marble1_subsurface, SR_marble1_specular, SR_marble1_roughness, SR_marble1_specularTint, SR_marble1_anisotropic, SR_marble1_sheen, SR_marble1_sheenTint, SR_marble1_clearcoat, SR_marble1_clearcoatGloss, SR_marble1_out1);
    material Marble_3D_out1 = material(vec3(0.0),vec3(0.0));
    surfacematerial(SR_marble1_out1, Marble_3D_out1);
    out1 = vec4(Marble_3D_out1.color, 1.0);
}
