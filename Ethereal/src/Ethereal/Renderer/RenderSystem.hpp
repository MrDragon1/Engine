#pragma once

#include "Ethereal/Core/Utils.hpp"
#include "Ethereal/Renderer/RenderResource.hpp"
#include "Ethereal/Renderer/RenderScene.hpp"
#include "Ethereal/Renderer/RenderPass.hpp"
#include "Ethereal/Scene/Scene.hpp"
#include "Common.hpp"
#include "Ethereal/Core/Timestep.hpp"

namespace Ethereal
{
    struct EntityDataForRenderSystem
    {
        size_t EntityID;
        GameObjectTransformDesc Transform;
        GameObjectMeshDesc Mesh;
        GameObjectMaterialDesc Material;
    };

    struct RenderSceneData
    {
        glm::mat4 ViewProjectionMatrix;
        std::vector<EntityDataForRenderSystem> EntitiesData;
        //TODO: Lights
    };

    class RenderSystem {
      public:
        RenderSystem();
        void Init();
        void Draw(Timestep ts);
        void UpdateRenderScene(const RenderSceneData& renderSceneData);


        Ref<RenderScene> m_RenderScene;
        Ref<RenderResource> m_RenderResource;  // All the mesh in scene
        Ref<MainCameraRenderPass> m_MainCameraRenderPass;
        // TODO: Ref<RenderContent>;
      private:

    };
}  // namespace Ethereal