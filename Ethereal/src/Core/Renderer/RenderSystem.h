#pragma once

#include "Base/Utils.h"
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Scene/Scene.h"
#include "Base/TimeStamp.h"

#include "Core/Renderer/RenderPass/EnvironmentMapRenderPass.h"
#include "Core/Renderer/RenderPass/MainCameraRenderPass.h"
#include "Core/Renderer/RenderPass/SkyboxRenderPass.h"
#include "Core/Renderer/RenderPass/ShadowMapRenderPass.h"
#include "Core/Renderer/RenderPass/CSMRenderPass.h"
#include "Core/Renderer/RenderPass/BloomRenderPass.h"

namespace Ethereal
{
    // Structs used in Buffers.glslh
    struct CameraData {
        Matrix4 ViewProjectionMatrix;
        Matrix4 InverseViewProjectionMatrix;
        Matrix4 ProjectionMatrix;
        Matrix4 InverseProjectionMatrix;
        Matrix4 ViewMatrix;
        Matrix4 InverseViewMatrix;
        Vector2 NDCToViewMul;
        Vector2 NDCToViewAdd;
        Vector2 DepthUnpackConsts;
        Vector2 CameraTanHalfFOV;
    };

    struct ShadowData {
        Matrix4 DirLightMatrices[4];
    };

    struct DirectionalLight {
        Vector3 Direction;
        float ShadowAmount;
        Vector3 Radiance;
        float Multiplier;
    };

    struct SceneData {
        DirectionalLight DirectionalLight;
        Vector3 CameraPosition;
        float EnvironmentMapIntensity;
    };

    struct RendererData {
        Vector4 CascadeSplits;
        int TilesCountX;
        bool ShowCascades;
        bool SoftShadows;
        float LightSize;
        float MaxShadowDistance;
        float ShadowFade;
        bool CascadeFading;
        float CascadeTransitionFade;
        bool ShowLightComplexity;
    };

    // TODO: Remove this
    struct RenderSceneData {
        Vector3 CameraPosition;
        float NearPlane;
        float FarPlane;
        float AspectRatio;
        float FOV;
        Ref<Environment> Environment;

        // TODO: Lights
        Vector3 DirectionalLightDir = Math::Normalize(Vector3(-0.352,0.270,-0.452));
    };

    struct ShaderCommonData{
        CameraData CameraData;
        ShadowData ShadowData;
        SceneData SceneData;
        RendererData RendererData;

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
        void SubmitRenderSceneData(const ShaderCommonData& data);
        void OnResize();
        void LoadProjectSettings();

        Ref<Environment> GetDefaultEnvironment() { return m_BuildinData->Environment; }

        uint32_t GetMainImageHeight() { return m_Height; };
        uint32_t GetMainImageWidth() { return m_Width; };
        uint64_t GetMainImage();
        uint64_t GetSkyboxImage() { return m_EnvironmentMapRenderPass->m_Texture->GetRendererID(); };
        int GetMousePicking(int x, int y);


        Ref<CSMRenderPass> GetCSMRenderPass() { return m_CSMRenderPass; }
        Ref<Environment>  GetEnv() { return m_Environment; }
      private:
        Ref<MainCameraRenderPass> m_MainCameraRenderPass;
        Ref<ShadowMapRenderPass> m_ShadowMapRenderPass;  // Don't use it for now.
        Ref<SkyboxRenderPass> m_SkyboxRenderPass;
        Ref<EnvironmentMapRenderPass> m_EnvironmentMapRenderPass;
        Ref<BloomRenderPass> m_BloomRenderPass;
        Ref<CSMRenderPass> m_CSMRenderPass;

        Ref<Environment> m_Environment;
        Ref<Texture> m_MainImage;
        uint32_t m_Height, m_Width;

        Ref<UniformBufferSet> m_UniformBufferSet;


        DrawLists* m_DrawLists;
        BuildinData* m_BuildinData;
    };
}  // namespace Ethereal