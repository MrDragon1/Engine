#pragma once

#include "pch.hpp"
#include "GameObject.hpp"
#include "EditorCamera.hpp"
#include "SceneCamera.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"
namespace Ethereal
{
    class RenderResource;

    class RenderScene {
      public:
        RenderScene() = default;
        RenderScene(const std::string& shaderPath);
        void UpdateVisiableMeshNode(const Ref<RenderResource>& RenderResource);

        void AddGameObject(Ref<GameObject>& gameObject);

        void BeginRender(const EditorCamera& camera);
        void BeginRender(const glm::mat4& projection, const glm::mat4& transform);

        void Clear();

      private:
        std::vector<Ref<GameObject>> m_GameObjects;
        std::vector<RenderMeshNode> m_Visiable_RenderMeshNode;  // only visiable scene
        Ref<Shader> m_Shader;

    };

}  // namespace Ethereal