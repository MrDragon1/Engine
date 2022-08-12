#pragma once

#include "Core/Utils.h"
#include "RenderPass.h"
#include "Mesh.h"
#include "Scene/Scene.h"
#include "Core/Timestep.h"

#include "Renderer/RenderPass/EnvironmentMapRenderPass.h"
#include "Renderer/RenderPass/MainCameraRenderPass.h"
#include "Renderer/RenderPass/SkyboxRenderPass.h"
#include "Renderer/RenderPass/ShadowMapRenderPass.h"

namespace Ethereal
{
    struct RenderSceneData {
        glm::mat4 ViewProjectionMatrix;
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        glm::vec3 CameraPosition;
        Ref<Environment> Environment;

        // TODO: Lights
    };

    struct BuildinData {
        Ref<Texture> WhiteTexture;
        Ref<Texture> BlackTexture;
        Ref<Texture> BRDFLutTexture;
        Ref<StaticMesh> Cube;
        Ref<Environment> Environment;
    };

    class RenderSystem : public RefCounted {
      public:
        RenderSystem();
        void Init();
        void Draw(Timestep ts);

        std::pair<Ref<TextureCube>, Ref<TextureCube>> CreateEnvironmentMap(const std::string& path);
        void SubmitStaticMesh(Ref<StaticMesh> staticMesh, Ref<MaterialTable> materialTabel, uint32_t EntityID,
                              const glm::mat4& transform = glm::mat4(1.0f));
        void SubmitRenderSceneData(const RenderSceneData& data);
        void OnResize();

        Ref<StaticMesh> GetCubeStaticMesh() { return m_BuildinData->Cube; }
        Ref<Texture> GetWhiteTexture() { return m_BuildinData->WhiteTexture; }
        Ref<Texture> GetBlackTexture() { return m_BuildinData->BlackTexture; }
        Ref<Texture> GetBRDFLutTexture() { return m_BuildinData->BRDFLutTexture; }
        Ref<Environment> GetDefaultEnvironment() { return m_BuildinData->Environment; }

        uint32_t GetMainImageHeight() { return m_Height; };
        uint32_t GetMainImageWidth() { return m_Width; };
        uint64_t GetMainImage();
        uint64_t GetSkyboxImage() { return m_EnvironmentMapRenderPass->m_Texture->GetRendererID(); };
        int GetMousePicking(int x, int y);

        Ref<MainCameraRenderPass> m_MainCameraRenderPass;
        Ref<ShadowMapRenderPass> m_ShadowMapRenderPass;
        Ref<SkyboxRenderPass> m_SkyboxRenderPass;
        Ref<EnvironmentMapRenderPass> m_EnvironmentMapRenderPass;
        // TODO: Ref<RenderContent>;
        uint32_t m_Height, m_Width;
        DrawLists* m_DrawLists;
        BuildinData* m_BuildinData;
        Ref<Environment> m_Environment;
    };
}  // namespace Ethereal