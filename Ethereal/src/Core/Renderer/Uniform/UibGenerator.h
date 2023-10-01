#pragma once
namespace Ethereal {

class BufferInterfaceBlock;

// Not used for now
class UibGenerator {
   public:
    static BufferInterfaceBlock const& GetViewUib() noexcept;
    static BufferInterfaceBlock const& GetRenderPrimitiveUib() noexcept;
    static BufferInterfaceBlock const& GetRenderPrimitiveBoneUib() noexcept;
    static BufferInterfaceBlock const& GetShadowUib() noexcept;
    static BufferInterfaceBlock const& GetLightUib() noexcept;
};

struct ViewUib {
    static constexpr std::string_view _name{"ViewUniform"};

    Vector3 CameraPosition;
    float EnvironmentMapIntensity;
    Vector4 ScissorNormalized;

    // Camera
    Matrix4 ViewProjectionMatrix;
    Matrix4 InverseViewProjectionMatrix;
    Matrix4 ProjectionMatrix;
    Matrix4 InverseProjectionMatrix;
    Matrix4 ViewMatrix;
    Matrix4 InverseViewMatrix;
    float FarPlane;
    float NearPlane;
    uint32_t padding[2];

    // Fog
    Vector4 FogColor;
    float FogStart;
    float FogDensity;
    float FogHeight;
    float FogHeightFalloff;
    float FogScatteringStart;
    float FogScatteringSize;
    uint32_t FogEnable = false;
    uint32_t FogFromIBL = false;

    Vector4 padding2[2];
};
static_assert(sizeof(ViewUib) % 64 == 0);

struct RenderPrimitiveUib {
    static constexpr std::string_view _name{"RenderPrimitiveUniform"};
    Matrix4 ModelMatrix;
    Vector4 Albedo;
    float Metallic;
    float Roughness;
    float Occlusion;
    float Emisstion;
    int UseMap;  // 1<<1: Albedo 1<<2: Normal 1<<3: Metallic 1<<4: Roughness 1<<5: Occlusion

    int EntityID;
    Vector2 padding;
    Vector4 padding2;
};
static_assert(sizeof(RenderPrimitiveUib) % 64 == 0);

struct RenderPrimitiveBoneUib {
    static constexpr std::string_view _name{"RenderPrimitiveBoneUniform"};
    Matrix4 BoneTransform[100];  // same size with shader
};
static_assert(sizeof(RenderPrimitiveBoneUib) % 64 == 0);

struct LightUib {
    static constexpr std::string_view _name{"LightUniform"};
    Vector3 Direction;
    float ShadowAmount;
    Vector3 Radiance;
    float Multiplier;
    Vector4 padding[2];
};
static_assert(sizeof(LightUib) % 64 == 0);

struct ShadowUib {
    static constexpr std::string_view _name{"ShadowUniform"};

    // TODO: A little waste of memory, but it's ok for now.
    // Only need 16 float values, we use the first value of Vector4 for now.
    Vector4 CascadeSplits[16];
    Matrix4 DirLightMatrices[16];
    uint32_t CascadeCount = 4;
    Vector3 padding;
    Vector4 padding2[3];
};
static_assert(sizeof(ShadowUib) % 64 == 0);
// struct DirectionalLight {
//     Vector3 Direction = Math::Normalize(Vector3(-0.352, 0.270, -0.452));
//     float ShadowAmount;
//     Vector3 Radiance = Vector3(1.0, 1.0, 1.0);
//     float Multiplier;
// };

}  // namespace Ethereal