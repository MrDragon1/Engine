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
    struct RenderSceneData {
        Matrix4 ViewProjectionMatrix;
        Matrix4 ViewMatrix;
        Matrix4 ProjectionMatrix;
        Vector3 CameraPosition;
        float NearPlane;
        float FarPlane;
        float AspectRatio;
        float FOV;
        Ref<Environment> Environment;

        // TODO: Lights
        Vector3 DirectionalLightDir = Math::Normalize(Vector3(20.0f, 50, 20.0f));
    };

    struct BuildinData {
        Ref<Texture> WhiteTexture;
        Ref<Texture> BlackTexture;
        Ref<Texture> BRDFLutTexture;
        Ref<StaticMesh> Cube;
        Ref<StaticMesh> Quad;
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
        void SubmitRenderSceneData(const RenderSceneData& data);
        void OnResize();
        void LoadProjectSettings();

        Ref<StaticMesh> GetCubeStaticMesh() { return m_BuildinData->Cube; }
        Ref<StaticMesh> GetQuadStaticMesh() { return m_BuildinData->Quad; }
        Ref<Texture> GetWhiteTexture() { return m_BuildinData->WhiteTexture; }
        Ref<Texture> GetBlackTexture() { return m_BuildinData->BlackTexture; }
        Ref<Texture> GetBRDFLutTexture() { return m_BuildinData->BRDFLutTexture; }
        Ref<Environment> GetDefaultEnvironment() { return m_BuildinData->Environment; }

        uint32_t GetMainImageHeight() { return m_Height; };
        uint32_t GetMainImageWidth() { return m_Width; };
        uint64_t GetMainImage();
        uint64_t GetSkyboxImage() { return m_EnvironmentMapRenderPass->m_Texture->GetRendererID(); };
        int GetMousePicking(int x, int y);

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

        DrawLists* m_DrawLists;
        BuildinData* m_BuildinData;
    };
}  // namespace Ethereal