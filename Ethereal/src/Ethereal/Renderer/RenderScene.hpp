#pragma once

#include "pch.hpp"
#include "GameObject.hpp"
#include "EditorCamera.hpp"
#include "SceneCamera.hpp"
#include "Shader.hpp"
#include "Framebuffer.hpp"
#include "Texture.hpp"
#include "Ethereal/Utils/UUIDAllocator.hpp"

namespace Ethereal
{
    class RenderResource;
    static constexpr size_t MaxTextureSlots = 32;
    class RenderScene {
      public:
        RenderScene() = default;
        void UpdateVisiableMeshNode(const Ref<RenderResource>& RenderResource);

        void AddGameObject(const Ref<GameObject>& gameObject);

        void SetViewProjectionMatrix(const glm::mat4& matrix);
        void SetVisiableNodeReference();

        void BeginRender();

        UUIDAllocator<GameObjectMeshDesc>& getMeshAssetIdAllocator() { return m_MeshAssetIdAllocator; };
        UUIDAllocator<GameObjectMaterialDesc>& getMaterialAssetIdAllocator() { return m_MaterialAssetIdAllocator; };
        void Clear();

      private:
        UUIDAllocator<GameObjectMeshDesc> m_MeshAssetIdAllocator;
        UUIDAllocator<GameObjectMaterialDesc> m_MaterialAssetIdAllocator;

        std::vector<Ref<GameObject>> m_GameObjects;
        std::vector<RenderNode> m_MainCameraVisiableNodes;
    };

}  // namespace Ethereal