#pragma once
namespace Ethereal {

class BufferInterfaceBlock;

class UibGenerator {
   public:
    static BufferInterfaceBlock const& GetCameraUib() noexcept;
    static BufferInterfaceBlock const& GetShadowUib() noexcept;
    static BufferInterfaceBlock const& GetSceneUib() noexcept;
    static BufferInterfaceBlock const& GetEditorUib() noexcept;
};

struct CameraUib {
    static constexpr std::string_view _name{"CameraUniform"};
    Matrix4 ViewProjectionMatrix;
    Matrix4 InverseViewProjectionMatrix;
    Matrix4 ProjectionMatrix;
    Matrix4 InverseProjectionMatrix;
    Matrix4 ViewMatrix;
    Matrix4 InverseViewMatrix;
    float FarPlane;
    float NearPlane;
    float padding[2];
};

struct ShadowUib {
    static constexpr std::string_view _name{"ShadowUniform"};

    uint32_t CascadeCount = 4;
    float padding1, padding2, padding3;
    // TODO: A little waste of memory, but it's ok for now.
    // Only need 16 float values, we use the first value of Vector4 for now.
    Vector4 CascadeSplits[16];
    Matrix4 DirLightMatrices[16];
};

struct SceneUib {
    static constexpr std::string_view _name{"SceneUniform"};
    // struct DirectionalLight {
    //     Vector3 Direction = Math::Normalize(Vector3(-0.352, 0.270, -0.452));
    //     float ShadowAmount;
    //     Vector3 Radiance = Vector3(1.0, 1.0, 1.0);
    //     float Multiplier;
    // };

    // DirectionalLight DirLight;

    Vector3 CameraPosition;
    float EnvironmentMapIntensity;
    Vector4 ScissorNormalized;

    // Fog
    Vector4 FogColor;
    float FogStart;
    float FogDensity;
    float FogHeight;
    float FogHeightFalloff;
    float FogScatteringStart;
    float FogScatteringSize;
    bool FogEnable = false;
    bool FogFromIBL = false;
};

struct EditorUib {
    static constexpr std::string_view _name{"EditorUniform"};
    uint32_t EntityID;
    float padding[3];
};

}  // namespace Ethereal