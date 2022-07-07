#pragma once

#include "Ethereal/Core/Utils.hpp"
#include "Ethereal/Renderer/RenderResource.hpp"
#include "Ethereal/Renderer/RenderScene.hpp"
#include "Ethereal/Scene/Scene.hpp"
#include "Common.hpp"
#include "Ethereal/Core/Timestep.hpp"

namespace Ethereal
{
    class RenderSystem {
      public:
        RenderSystem();
        void OnUpdateEditor(Timestep ts, const Ref<Scene>& scene, const EditorCamera& camera);
        void OnUpdateRuntime(Timestep ts, const Ref<Scene>& scene);

        Ref<RenderScene> m_RenderScene;
        Ref<RenderResource> m_RenderResource;  // All the mesh in scene
        // TODO: Ref<RenderContent>;
    };
}  // namespace Ethereal