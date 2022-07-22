#pragma once

#include "Ethereal/Core/Utils.hpp"
#include "Ethereal/Renderer/RenderResource.hpp"
#include "Ethereal/Renderer/RenderScene.hpp"
#include "Ethereal/Renderer/RenderPass.hpp"
#include "Ethereal/Scene/Scene.hpp"
#include "Common.hpp"
#include "Ethereal/Core/Timestep.hpp"

#include "Ethereal/Renderer/RenderPass/EnvironmentMapRenderPass.hpp"
#include "Ethereal/Renderer/RenderPass/MainCameraRenderPass.hpp"
#include "Ethereal/Renderer/RenderPass/SkyboxRenderPass.hpp"
#include "Ethereal/Renderer/RenderPass/ShadowMapRenderPass.hpp"

namespace Ethereal
{
    struct EntityDataForRenderSystem {
        size_t EntityID;
        GameObjectTransformDesc Transform;
        GameObjectMeshDesc Mesh;
        GameObjectMaterialDesc Material;
    };

    struct RenderSceneData {
        glm::mat4 ViewProjectionMatrix;
        glm::mat4 ViewMatrix;
        glm::mat4 ProjectionMatrix;
        glm::vec3 CameraPosition;
        SkyboxData Skybox;

        std::vector<EntityDataForRenderSystem> EntitiesData;
        // TODO: Lights
    };

    class RenderSystem {
      public:
        RenderSystem();
        void Init();
        void Draw(Timestep ts);
        void UpdateRenderScene(const RenderSceneData& renderSceneData);

        void OnResize(int width, int height);

        uint32_t GetMainImageHeight() { return m_Height; };
        uint32_t GetMainImageWidth() { return m_Width; };
        uint64_t GetMainImage();
        uint64_t GetSkyboxImage() { return m_EnvironmentMapRenderPass->m_BackgroundTexture->GetRendererID(); };
        int GetMousePicking(int x, int y);

        Ref<RenderScene> m_RenderScene;
        Ref<RenderResource> m_RenderResource;  // All the mesh in scene
        Ref<MainCameraRenderPass> m_MainCameraRenderPass;
        Ref<ShadowMapRenderPass> m_ShadowMapRenderPass;
        Ref<SkyboxRenderPass> m_SkyboxRenderPass;
        Ref<EnvironmentMapRenderPass> m_EnvironmentMapRenderPass;
        // TODO: Ref<RenderContent>;
        uint32_t m_Height, m_Width;

      private:
    };
}  // namespace Ethereal