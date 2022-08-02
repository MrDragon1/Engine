#pragma once

#include "Core/Utils.h"
#include "Renderer/RenderResource.h"
#include "Renderer/RenderScene.h"
#include "Renderer/RenderPass.h"
#include "Scene/Scene.h"
#include "Common.h"
#include "Core/Timestep.h"

#include "Renderer/RenderPass/EnvironmentMapRenderPass.h"
#include "Renderer/RenderPass/MainCameraRenderPass.h"
#include "Renderer/RenderPass/SkyboxRenderPass.h"
#include "Renderer/RenderPass/ShadowMapRenderPass.h"

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

    class RenderSystem : public RefCounted{
      public:
        RenderSystem();
        void Init();
        void Draw(Timestep ts);
        void UpdateRenderScene(const RenderSceneData& renderSceneData);

        void OnResize();

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