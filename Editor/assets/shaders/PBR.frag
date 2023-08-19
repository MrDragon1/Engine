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

//------------------------------------------------------------------------------
// BRDF configuration
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Common math
//------------------------------------------------------------------------------

/** @public-api */
#define PI                 3.14159265359
/** @public-api */
#define HALF_PI            1.570796327

#define MEDIUMP_FLT_MAX    65504.0
#define MEDIUMP_FLT_MIN    0.00006103515625

#ifdef TARGET_MOBILE
#define FLT_EPS            MEDIUMP_FLT_MIN
#define saturateMediump(x) min(x, MEDIUMP_FLT_MAX)
#else
#define FLT_EPS            1e-5
#define saturateMediump(x) x
#endif

#define saturate(x)        clamp(x, 0.0, 1.0)
#define atan2(x, y)        atan(y, x)

//------------------------------------------------------------------------------
// Scalar operations
//------------------------------------------------------------------------------

/**
 * Computes x^5 using only multiply operations.
 *
 * @public-api
 */
float pow5(float x) {
    float x2 = x * x;
    return x2 * x2 * x;
}

/**
 * Computes x^2 as a single multiplication.
 *
 * @public-api
 */
float sq(float x) {
    return x * x;
}

//------------------------------------------------------------------------------
// Vector operations
//------------------------------------------------------------------------------

/**
 * Returns the maximum component of the specified vector.
 *
 * @public-api
 */
float max3(const vec3 v) {
    return max(v.x, max(v.y, v.z));
}

float vmax(const vec2 v) {
    return max(v.x, v.y);
}

float vmax(const vec3 v) {
    return max(v.x, max(v.y, v.z));
}

float vmax(const vec4 v) {
    return max(max(v.x, v.y), max(v.y, v.z));
}

/**
 * Returns the minimum component of the specified vector.
 *
 * @public-api
 */
float min3(const vec3 v) {
    return min(v.x, min(v.y, v.z));
}

float vmin(const vec2 v) {
    return min(v.x, v.y);
}

float vmin(const vec3 v) {
    return min(v.x, min(v.y, v.z));
}

float vmin(const vec4 v) {
    return min(min(v.x, v.y), min(v.y, v.z));
}

//------------------------------------------------------------------------------
// Trigonometry
//------------------------------------------------------------------------------

/**
 * Approximates acos(x) with a max absolute error of 9.0x10^-3.
 * Valid in the range -1..1.
 */
float acosFast(float x) {
    // Lagarde 2014, "Inverse trigonometric functions GPU optimization for AMD GCN architecture"
    // This is the approximation of degree 1, with a max absolute error of 9.0x10^-3
    float y = abs(x);
    float p = -0.1565827 * y + 1.570796;
    p *= sqrt(1.0 - y);
    return x >= 0.0 ? p : PI - p;
}

/**
 * Approximates acos(x) with a max absolute error of 9.0x10^-3.
 * Valid only in the range 0..1.
 */
float acosFastPositive(float x) {
    float p = -0.1565827 * x + 1.570796;
    return p * sqrt(1.0 - x);
}

//------------------------------------------------------------------------------
// Matrix and quaternion operations
//------------------------------------------------------------------------------

/**
 * Multiplies the specified 3-component vector by the 4x4 matrix (m * v) in
 * high precision.
 *
 * @public-api
 */
highp vec4 mulMat4x4Float3(const highp mat4 m, const highp vec3 v) {
    return v.x * m[0] + (v.y * m[1] + (v.z * m[2] + m[3]));
}

/**
 * Multiplies the specified 3-component vector by the 3x3 matrix (m * v) in
 * high precision.
 *
 * @public-api
 */
highp vec3 mulMat3x3Float3(const highp mat4 m, const highp vec3 v) {
    return v.x * m[0].xyz + (v.y * m[1].xyz + (v.z * m[2].xyz));
}

/**
 * Extracts the normal vector of the tangent frame encoded in the specified quaternion.
 */
void toTangentFrame(const highp vec4 q, out highp vec3 n) {
    n = vec3( 0.0,  0.0,  1.0) +
        vec3( 2.0, -2.0, -2.0) * q.x * q.zwx +
        vec3( 2.0,  2.0, -2.0) * q.y * q.wzy;
}

/**
 * Extracts the normal and tangent vectors of the tangent frame encoded in the
 * specified quaternion.
 */
void toTangentFrame(const highp vec4 q, out highp vec3 n, out highp vec3 t) {
    toTangentFrame(q, n);
    t = vec3( 1.0,  0.0,  0.0) +
        vec3(-2.0,  2.0, -2.0) * q.y * q.yxw +
        vec3(-2.0,  2.0,  2.0) * q.z * q.zwx;
}

highp mat3 cofactor(const highp mat3 m) {
    highp float a = m[0][0];
    highp float b = m[1][0];
    highp float c = m[2][0];
    highp float d = m[0][1];
    highp float e = m[1][1];
    highp float f = m[2][1];
    highp float g = m[0][2];
    highp float h = m[1][2];
    highp float i = m[2][2];

    highp mat3 cof;
    cof[0][0] = e * i - f * h;
    cof[0][1] = c * h - b * i;
    cof[0][2] = b * f - c * e;
    cof[1][0] = f * g - d * i;
    cof[1][1] = a * i - c * g;
    cof[1][2] = c * d - a * f;
    cof[2][0] = d * h - e * g;
    cof[2][1] = b * g - a * h;
    cof[2][2] = a * e - b * d;
    return cof;
}

//------------------------------------------------------------------------------
// Random
//------------------------------------------------------------------------------

/*
 * Random number between 0 and 1, using interleaved gradient noise.
 * w must not be normalized (e.g. window coordinates)
 */
float interleavedGradientNoise(highp vec2 w) {
    const vec3 m = vec3(0.06711056, 0.00583715, 52.9829189);
    return fract(m.z * fract(dot(w, m.xy)));
}

// Diffuse BRDFs
#define DIFFUSE_LAMBERT             0
#define DIFFUSE_BURLEY              1

// Specular BRDF
// Normal distribution functions
#define SPECULAR_D_GGX              0

// Visibility functions
#define SPECULAR_V_SMITH_GGX        0
#define SPECULAR_V_SMITH_GGX_FAST   1
#define SPECULAR_V_GGX_ANISOTROPIC  2
#define SPECULAR_V_KELEMEN          3
#define SPECULAR_V_NEUBELT          4

// Fresnel functions
#define SPECULAR_F_SCHLICK          0

#define BRDF_DIFFUSE                DIFFUSE_LAMBERT

#define BRDF_SPECULAR_D             SPECULAR_D_GGX
#define BRDF_SPECULAR_V             SPECULAR_V_SMITH_GGX_FAST
#define BRDF_SPECULAR_F             SPECULAR_F_SCHLICK

//------------------------------------------------------------------------------
// Specular BRDF implementations
//------------------------------------------------------------------------------

float D_GGX(float roughness, float NoH, const vec3 h) {
    // Walter et al. 2007, "Microfacet Models for Refraction through Rough Surfaces"

    // In mediump, there are two problems computing 1.0 - NoH^2
    // 1) 1.0 - NoH^2 suffers floating point cancellation when NoH^2 is close to 1 (highlights)
    // 2) NoH doesn't have enough precision around 1.0
    // Both problem can be fixed by computing 1-NoH^2 in highp and providing NoH in highp as well

    // However, we can do better using Lagrange's identity:
    //      ||a x b||^2 = ||a||^2 ||b||^2 - (a . b)^2
    // since N and H are unit vectors: ||N x H||^2 = 1.0 - NoH^2
    // This computes 1.0 - NoH^2 directly (which is close to zero in the highlights and has
    // enough precision).
    // Overall this yields better performance, keeping all computations in mediump

    float oneMinusNoHSquared = 1.0 - NoH * NoH;

    float a = NoH * roughness;
    float k = roughness / (oneMinusNoHSquared + a * a);
    float d = k * k * (1.0 / PI);
    return saturateMediump(d);
}

float D_Charlie(float roughness, float NoH) {
    // Estevez and Kulla 2017, "Production Friendly Microfacet Sheen BRDF"
    float invAlpha  = 1.0 / roughness;
    float cos2h = NoH * NoH;
    float sin2h = max(1.0 - cos2h, 0.0078125); // 2^(-14/2), so sin2h^2 > 0 in fp16
    return (2.0 + invAlpha) * pow(sin2h, invAlpha * 0.5) / (2.0 * PI);
}

float V_SmithGGXCorrelated(float roughness, float NoV, float NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    float a2 = roughness * roughness;
    // TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
    float lambdaV = NoL * sqrt((NoV - a2 * NoV) * NoV + a2);
    float lambdaL = NoV * sqrt((NoL - a2 * NoL) * NoL + a2);
    float v = 0.5 / (lambdaV + lambdaL);
    // a2=0 => v = 1 / 4*NoL*NoV   => min=1/4, max=+inf
    // a2=1 => v = 1 / 2*(NoL+NoV) => min=1/4, max=+inf
    // clamp to the maximum value representable in mediump
    return saturateMediump(v);
}

float V_SmithGGXCorrelated_Fast(float roughness, float NoV, float NoL) {
    // Hammon 2017, "PBR Diffuse Lighting for GGX+Smith Microsurfaces"
    float v = 0.5 / mix(2.0 * NoL * NoV, NoL + NoV, roughness);
    return saturateMediump(v);
}

float V_SmithGGXCorrelated_Anisotropic(float at, float ab, float ToV, float BoV,
        float ToL, float BoL, float NoV, float NoL) {
    // Heitz 2014, "Understanding the Masking-Shadowing Function in Microfacet-Based BRDFs"
    // TODO: lambdaV can be pre-computed for all the lights, it should be moved out of this function
    float lambdaV = NoL * length(vec3(at * ToV, ab * BoV, NoV));
    float lambdaL = NoV * length(vec3(at * ToL, ab * BoL, NoL));
    float v = 0.5 / (lambdaV + lambdaL);
    return saturateMediump(v);
}

float V_Kelemen(float LoH) {
    // Kelemen 2001, "A Microfacet Based Coupled Specular-Matte BRDF Model with Importance Sampling"
    return saturateMediump(0.25 / (LoH * LoH));
}

float V_Neubelt(float NoV, float NoL) {
    // Neubelt and Pettineo 2013, "Crafting a Next-gen Material Pipeline for The Order: 1886"
    return saturateMediump(1.0 / (4.0 * (NoL + NoV - NoL * NoV)));
}

vec3 F_Schlick(const vec3 f0, float f90, float VoH) {
    // Schlick 1994, "An Inexpensive BRDF Model for Physically-Based Rendering"
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

vec3 F_Schlick(const vec3 f0, float VoH) {
    float f = pow(1.0 - VoH, 5.0);
    return f + f0 * (1.0 - f);
}

float F_Schlick(float f0, float f90, float VoH) {
    return f0 + (f90 - f0) * pow5(1.0 - VoH);
}

//------------------------------------------------------------------------------
// Specular BRDF dispatch
//------------------------------------------------------------------------------

float distribution(float roughness, float NoH, const vec3 h) {
#if BRDF_SPECULAR_D == SPECULAR_D_GGX
    return D_GGX(roughness, NoH, h);
#endif
}

float visibility(float roughness, float NoV, float NoL) {
#if BRDF_SPECULAR_V == SPECULAR_V_SMITH_GGX
    return V_SmithGGXCorrelated(roughness, NoV, NoL);
#elif BRDF_SPECULAR_V == SPECULAR_V_SMITH_GGX_FAST
    return V_SmithGGXCorrelated_Fast(roughness, NoV, NoL);
#endif
}

vec3 fresnel(const vec3 f0, float LoH) {
#if BRDF_SPECULAR_F == SPECULAR_F_SCHLICK
    float f90 = saturate(dot(f0, vec3(50.0 * 0.33)));
    return F_Schlick(f0, f90, LoH);
#endif
}

//------------------------------------------------------------------------------
// Diffuse BRDF implementations
//------------------------------------------------------------------------------

float Fd_Lambert() {
    return 1.0 / PI;
}

float Fd_Burley(float roughness, float NoV, float NoL, float LoH) {
    // Burley 2012, "Physically-Based Shading at Disney"
    float f90 = 0.5 + 2.0 * roughness * LoH * LoH;
    float lightScatter = F_Schlick(1.0, f90, NoL);
    float viewScatter  = F_Schlick(1.0, f90, NoV);
    return lightScatter * viewScatter * (1.0 / PI);
}

// Energy conserving wrap diffuse term, does *not* include the divide by pi
float Fd_Wrap(float NoL, float w) {
    return saturate((NoL + w) / sq(1.0 + w));
}

//------------------------------------------------------------------------------
// Diffuse BRDF dispatch
//------------------------------------------------------------------------------

float diffuse(float roughness, float NoV, float NoL, float LoH) {
#if BRDF_DIFFUSE == DIFFUSE_LAMBERT
    return Fd_Lambert();
#elif BRDF_DIFFUSE == DIFFUSE_BURLEY
    return Fd_Burley(roughness, NoV, NoL, LoH);
#endif
}
// Poisson disk generated with 'poisson-disk-generator' tool from
// https://github.com/corporateshark/poisson-disk-generator by Sergey Kosarevsky
/*const*/ mediump vec2 poissonDisk[64] = vec2[]( // don't use 'const' b/c of OSX GL compiler bug
vec2(0.511749, 0.547686), vec2(0.58929, 0.257224), vec2(0.165018, 0.57663), vec2(0.407692, 0.742285),
vec2(0.707012, 0.646523), vec2(0.31463, 0.466825), vec2(0.801257, 0.485186), vec2(0.418136, 0.146517),
vec2(0.579889, 0.0368284), vec2(0.79801, 0.140114), vec2(-0.0413185, 0.371455), vec2(-0.0529108, 0.627352),
vec2(0.0821375, 0.882071), vec2(0.17308, 0.301207), vec2(-0.120452, 0.867216), vec2(0.371096, 0.916454),
vec2(-0.178381, 0.146101), vec2(-0.276489, 0.550525), vec2(0.12542, 0.126643), vec2(-0.296654, 0.286879),
vec2(0.261744, -0.00604975), vec2(-0.213417, 0.715776), vec2(0.425684, -0.153211), vec2(-0.480054, 0.321357),
vec2(-0.0717878, -0.0250567), vec2(-0.328775, -0.169666), vec2(-0.394923, 0.130802), vec2(-0.553681, -0.176777),
vec2(-0.722615, 0.120616), vec2(-0.693065, 0.309017), vec2(0.603193, 0.791471), vec2(-0.0754941, -0.297988),
vec2(0.109303, -0.156472), vec2(0.260605, -0.280111), vec2(0.129731, -0.487954), vec2(-0.537315, 0.520494),
vec2(-0.42758, 0.800607), vec2(0.77309, -0.0728102), vec2(0.908777, 0.328356), vec2(0.985341, 0.0759158),
vec2(0.947536, -0.11837), vec2(-0.103315, -0.610747), vec2(0.337171, -0.584), vec2(0.210919, -0.720055),
vec2(0.41894, -0.36769), vec2(-0.254228, -0.49368), vec2(-0.428562, -0.404037), vec2(-0.831732, -0.189615),
vec2(-0.922642, 0.0888026), vec2(-0.865914, 0.427795), vec2(0.706117, -0.311662), vec2(0.545465, -0.520942),
vec2(-0.695738, 0.664492), vec2(0.389421, -0.899007), vec2(0.48842, -0.708054), vec2(0.760298, -0.62735),
vec2(-0.390788, -0.707388), vec2(-0.591046, -0.686721), vec2(-0.769903, -0.413775), vec2(-0.604457, -0.502571),
vec2(-0.557234, 0.00451362), vec2(0.147572, -0.924353), vec2(-0.0662488, -0.892081), vec2(0.863832, -0.407206)
);

float getPenumbraLs(const bool DIRECTIONAL, const uint index, const highp float zLight) {
    float penumbra;
    // This conditional is resolved at compile time
    if (DIRECTIONAL) {
        penumbra = 0.5;
    } else {
        // the penumbra radius depends on the light-space z for spotlights
        penumbra = 0.5 / zLight;
    }
    return penumbra;
}

mat2 getRandomRotationMatrix(highp vec2 fragCoord) {
    // rotate the poisson disk randomly
    fragCoord += 0 ; // 0 when TAA is not usedvec2 (frameUniforms.temporalNoise)
    float randomAngle = interleavedGradientNoise(fragCoord) * (2.0 * PI);
    vec2 randomBase = vec2(cos(randomAngle), sin(randomAngle));
    mat2 R = mat2(randomBase.x, randomBase.y, -randomBase.y, randomBase.x);
    return R;
}

highp vec2 computeReceiverPlaneDepthBias(const highp vec3 position) {
    // see: GDC '06: Shadow Mapping: GPU-based Tips and Techniques
    // Chain rule to compute dz/du and dz/dv
    // |dz/du|   |du/dx du/dy|^-T   |dz/dx|
    // |dz/dv| = |dv/dx dv/dy|    * |dz/dy|
    highp vec3 duvz_dx = dFdx(position);
    highp vec3 duvz_dy = dFdy(position);
    highp vec2 dz_duv = inverse(transpose(mat2(duvz_dx.xy, duvz_dy.xy))) * vec2(duvz_dx.z, duvz_dy.z);
    return dz_duv;
}

float getPenumbraRatio(const bool DIRECTIONAL, const uint index,
        float z_receiver, float z_blocker) {
    // z_receiver/z_blocker are not linear depths (i.e. they're not distances)
    // Penumbra ratio for PCSS is given by:  pr = (d_receiver - d_blocker) / d_blocker
    float penumbraRatio;
    if (DIRECTIONAL) {
        // TODO: take lispsm into account
        // For directional lights, the depths are linear but depend on the position (because of LiSPSM).
        // With:        z_linear = f + z * (n - f)
        // We get:      (r-b)/b ==> (f/(n-f) + r_linear) / (f/(n-f) + b_linear) - 1
        // Assuming f>>n and ignoring LISPSM, we get:
        penumbraRatio = (z_blocker - z_receiver) / (1.0 - z_blocker);
    }
    else {
        penumbraRatio = 1.0f; //TODO: update this
    }
    return penumbraRatio * 1.0f;
}

float filterPCSS(const mediump sampler2DArray map,
const highp vec4 scissorNormalized,
const highp vec2 size,
const highp vec2 uv, const float z_rec, const uint layer,
const highp vec2 filterRadii, const mat2 R, const highp vec2 dz_duv,
const uint tapCount) {

    float occludedCount = 0.0;
    for (uint i = 0u; i < tapCount; i++) {
        highp vec2 duv = R * (poissonDisk[i] * filterRadii);

        // sample the shadow map with a 2x2 PCF, this helps a lot in low resolution areas
        vec4 d;
        highp vec2 tc = clamp(uv + duv, scissorNormalized.xy, scissorNormalized.zw);
        highp vec2 st = tc.xy * size - 0.5;
        highp vec2 grad = fract(st);

        // we must use texelFetchOffset before texelLodOffset filters
        d[0] = texelFetchOffset(map, ivec3(st, layer), 0, ivec2(0, 1)).r;
        d[1] = texelFetchOffset(map, ivec3(st, layer), 0, ivec2(1, 1)).r;
        d[2] = texelFetchOffset(map, ivec3(st, layer), 0, ivec2(1, 0)).r;
        d[3] = texelFetchOffset(map, ivec3(st, layer), 0, ivec2(0, 0)).r;

        // receiver plane depth bias
        float z_bias = dot(dz_duv, duv);
        vec4 dz = d - vec4(z_rec); // dz>0 when blocker is between receiver and light
        vec4 pcf = step(z_bias, dz);
        occludedCount += mix(mix(pcf.w, pcf.z, grad.x), mix(pcf.x, pcf.y, grad.x), grad.y);
    }
    return occludedCount * (1.0 / float(tapCount));
}

void blockerSearchAndFilter(out float occludedCount, out float z_occSum,
const mediump sampler2DArray map, const highp vec4 scissorNormalized, const highp vec2 uv,
const float z_rec, const uint layer,
const highp vec2 filterRadii, const mat2 R, const highp vec2 dz_duv,
const uint tapCount) {
    occludedCount = 0.0;
    z_occSum = 0.0;
    for (uint i = 0u; i < tapCount; i++) {
        highp vec2 duv = R * (poissonDisk[i] * filterRadii);
        highp vec2 tc = clamp(uv + duv, scissorNormalized.xy, scissorNormalized.zw);

        float z_occ = textureLod(map, vec3(tc, layer), 0.0).r;

        // note: z_occ and z_rec are not necessarily linear here, comparing them is always okay for
        // the regular PCF, but the "distance" is meaningless unless they are actually linear
        // (e.g.: for the directional light).
        // Either way, if we assume that all the samples are close to each other we can take their
        // average regardless, and the average depth value of the occluders
        // becomes: z_occSum / occludedCount.

        // receiver plane depth bias
        float z_bias = dot(dz_duv, duv);
        float dz = z_occ - z_rec; // dz>0 when blocker is between receiver and light
        float occluded = step(z_bias, dz);
        occludedCount += occluded;
        z_occSum += z_occ * occluded;
    }
}

float ShadowSample_PCSS(const bool DIRECTIONAL,
        const mediump sampler2DArray map,
        const highp vec4 scissorNormalized,
        const uint layer, const uint index,
        const highp vec4 shadowPosition, const highp float zLight) {
    highp vec2 size = vec2(textureSize(map, 0));
    highp vec2 texelSize = vec2(1.0) / size;
    highp vec3 position = shadowPosition.xyz * (1.0 / shadowPosition.w);
    position = position * 0.5 + 0.5;
    // We need to use the shadow receiver plane depth bias to combat shadow acne due to the
    // large kernel.
    highp vec2 dz_duv = computeReceiverPlaneDepthBias(position);

    float penumbra = getPenumbraLs(DIRECTIONAL, index, zLight);

    // rotate the poisson disk randomly
    mat2 R = getRandomRotationMatrix(gl_FragCoord.xy);

    float occludedCount = 0.0;
    float z_occSum = 0.0;

    blockerSearchAndFilter(occludedCount, z_occSum,
            map, scissorNormalized, position.xy, position.z, layer, texelSize * penumbra, R, dz_duv,
            16u);

    // early exit if there is no occluders at all, also avoids a divide-by-zero below.
    if (z_occSum == 0.0) {
        return 1.0;
    }

    float penumbraRatio = getPenumbraRatio(DIRECTIONAL, index, position.z, z_occSum / occludedCount);

    float percentageOccluded = filterPCSS(map, scissorNormalized, size,
            position.xy, position.z, layer,
            texelSize * (penumbra * penumbraRatio),
            R, dz_duv, 16u);

    return 1.0 - percentageOccluded;
}


layout(location = 0) out vec4 FragColor;
layout(location = 1) out int EntityID;

layout(location = 0) in vec3 v_WorldPos;
layout(location = 1) in vec3 v_Normal;
layout(location = 2) in vec2 v_TexCoord;

layout(set=1,binding = 10) uniform sampler2D u_AlbedoMap;
layout(set=1,binding = 11) uniform sampler2D u_NormalMap;
layout(set=1,binding = 12) uniform sampler2D u_MetallicMap;
layout(set=1,binding = 13) uniform sampler2D u_RoughnessMap;
layout(set=1,binding = 14) uniform sampler2D u_OcclusionMap;

// IBL
layout(set=1,binding = 15) uniform sampler2D u_BRDFLUT;
layout(set=1,binding = 17) uniform samplerCube u_IrradianceMap;
layout(set=1,binding = 16) uniform samplerCube u_PrefilterMap;

layout(set=1,binding = 18) uniform sampler2DArray u_ShadowMap;


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
    // uint cascade = getShadowCascade();
    // highp vec4 shadowPosition = getCascadeLightSpacePosition(cascade);
    // visibility = 1.0 - shadow(true, u_ShadowMap, cascade, shadowPosition, 0.0f);

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
    EntityID = u_View.EntityID;
}
