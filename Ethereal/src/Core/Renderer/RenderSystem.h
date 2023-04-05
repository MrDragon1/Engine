#pragma once

#include "Base/TimeStamp.h"
#include "Base/Utils.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RenderPass/BloomRenderPass.h"
#include "Core/Renderer/RenderPass/CSMRenderPass.h"
#include "Core/Renderer/RenderPass/EnvironmentMapRenderPass.h"
#include "Core/Renderer/RenderPass/MainCameraRenderPass.h"
#include "Core/Renderer/RenderPass/ShadowMapRenderPass.h"
#include "Core/Renderer/RenderPass/SkyboxRenderPass.h"
#include "Core/Scene/Scene.h"

// For test backend
#include "Platform/Windows/Backend/Driver.h";
#include "Uniform/TypedUniform.h"
#include "Uniform/UibGenerator.h"
namespace Ethereal {
// Structs used in Buffers.glslh
struct CameraData {
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

// std140 layout
struct ShadowData {
    uint32_t CascadeCount = 4;
    float padding1, padding2, padding3;
    // TODO: A little waste of memory, but it's ok for now.
    // Only need 16 float values, we use the first value of Vector4 for now.
    Vector4 CascadeSplits[16];
    Matrix4 DirLightMatrices[16];
};

struct DirectionalLight {
    Vector3 Direction = Math::Normalize(Vector3(-0.352, 0.270, -0.452));
    float ShadowAmount;
    Vector3 Radiance = Vector3(1.0, 1.0, 1.0);
    float Multiplier;
};

struct SceneData {
    DirectionalLight DirectionalLight;
    Vector3 CameraPosition;
    float EnvironmentMapIntensity;
    Vector4 ScissorNormalized;
};

struct RendererData {
    int EntityID;
    float padding[3];
};

// TODO: Move to other place
struct MaterialData {
    Vector4 u_Albedo;
    float u_Metallic;
    float u_Roughness;
    float u_Occlusion;
    float u_Emisstion;
    int u_UseMap;  // 1<<1: Albedo 1<<2: Normal 1<<3: Metallic 1<<4: Roughness 1<<5: Occlusion
};

struct FogData {
    Vector4 Color;
    float Start;
    float Density;
    float Height;
    float HeightFalloff;
    float ScatteringStart;
    float ScatteringSize;
    bool Enable = false;
    bool FromIBL = false;
};

// TODO: Remove this
struct RenderSceneData {
    float AspectRatio;
    float FOV;
    Ref<Environment> Environment;
};

struct ShaderCommonData {
    CameraData CameraData;
    ShadowData ShadowData;
    SceneData SceneData;
    RendererData RendererData;
    MaterialData MaterialData;
    FogData FogData;

    RenderSceneData RenderSceneData;
};

// TODO: Move this to RenderResource
struct BuildinData {
    Ref<Environment> Environment;
};

class RenderSystem : public RefCounted {
   public:
    RenderSystem();
    void Init();
    void Draw(TimeStamp ts);

    std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& path);
    void SubmitStaticMesh(Ref<StaticMesh> staticMesh, Ref<MaterialTable> materialTabel, uint32_t EntityID,
                          const Matrix4& transform = Matrix4::IDENTITY);
    void SubmitMesh(Ref<Mesh> mesh, Ref<MaterialTable> materialTabel, uint32_t EntityID, const Matrix4& transform = Matrix4::IDENTITY);
    void SubmitRenderSceneData();
    void OnResize();
    void LoadProjectSettings();
    void UpdateUniformData();
    Ref<Environment> GetDefaultEnvironment() { return m_BuildinData->Environment; }

    uint32_t GetMainImageHeight() { return m_Height; };
    uint32_t GetMainImageWidth() { return m_Width; };
    uint64_t GetMainImage();
    uint64_t GetSkyboxImage() { return m_EnvironmentMapRenderPass->m_Texture->GetRendererID(); };
    int GetMousePicking(int x, int y);

    Ref<CSMRenderPass> GetCSMRenderPass() { return m_CSMRenderPass; }
    Ref<Environment> GetEnv() { return m_Environment; }
    ShaderCommonData& GetShaderCommonData() { return m_ShaderCommonData; }
    Ref<UniformBufferSet> GetUniformBufferSet() { return m_UniformBufferSet; }

   private:
    Ref<MainCameraRenderPass> m_MainCameraRenderPass;
    Ref<ShadowMapRenderPass> m_ShadowMapRenderPass;  // Don't use it for now.
    Ref<SkyboxRenderPass> m_SkyboxRenderPass;
    Ref<EnvironmentMapRenderPass> m_EnvironmentMapRenderPass;
    Ref<BloomRenderPass> m_BloomRenderPass;
    Ref<CSMRenderPass> m_CSMRenderPass;

    Ref<Environment> m_Environment;
    Ref<Ethereal::Texture> m_MainImage;
    uint32_t m_Height, m_Width;

    Ref<UniformBufferSet> m_UniformBufferSet;

    ShaderCommonData m_ShaderCommonData;

    DrawLists* m_DrawLists;
    BuildinData* m_BuildinData;

    Backend::Driver* mDriver;
    Ref<Backend::RenderPrimitive> mTrianglePrimitive;
    Backend::PipelineState mPipelineState;
    Ref<Backend::SamplerGroup> mSamplerGroup;
    Ref<Backend::Texture> mTextureA, mTextureB;
    Ref<Backend::RenderTarget> mRenderTarget;
    Ref<Backend::BufferObject> mUniformBuffer;

    TypedUniform<CameraUib> mCameraUib;
    TypedUniform<ShadowUib> mShadowUib;
    TypedUniform<SceneUib> mSceneUib;
    TypedUniform<EditorUib> mEditorUib;
    Ref<Backend::BufferObject> mCameraUB;
    Ref<Backend::BufferObject> mShadowUB;
    Ref<Backend::BufferObject> mSceneUB;
    Ref<Backend::BufferObject> mEditorUB;
};
}  // namespace Ethereal