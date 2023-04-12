#pragma once
namespace Ethereal {

class BufferInterfaceBlock;

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

    // Editor
    uint32_t EntityID;
    uint32_t padding;

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
};

struct RenderPrimitiveUib {
    static constexpr std::string_view _name{"RenderPrimitiveUniform"};
    Matrix4 ModelMatrix;
    Vector4 Albedo;
    float Metallic;
    float Roughness;
    float Occlusion;
    float Emisstion;
    int UseMap;  // 1<<1: Albedo 1<<2: Normal 1<<3: Metallic 1<<4: Roughness 1<<5: Occlusion
};

struct RenderPrimitiveBoneUib {
    static constexpr std::string_view _name{"RenderPrimitiveBoneUniform"};
    Matrix4 BoneTransform[100];  // same size with shader
};

struct LightUib {
    static constexpr std::string_view _name{"LightUniform"};
    Vector3 Direction;
    float ShadowAmount;
    Vector3 Radiance;
    float Multiplier;
};

struct ShadowUib {
    static constexpr std::string_view _name{"ShadowUniform"};

    // TODO: A little waste of memory, but it's ok for now.
    // Only need 16 float values, we use the first value of Vector4 for now.
    Vector4 CascadeSplits[16];
    Matrix4 DirLightMatrices[16];
    uint32_t CascadeCount = 4;
    float padding1, padding2, padding3;
};

// struct DirectionalLight {
//     Vector3 Direction = Math::Normalize(Vector3(-0.352, 0.270, -0.452));
//     float ShadowAmount;
//     Vector3 Radiance = Vector3(1.0, 1.0, 1.0);
//     float Multiplier;
// };

}  // namespace Ethereal