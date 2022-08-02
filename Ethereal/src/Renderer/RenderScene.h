#pragma once

//#include "pch.h"
#include "GameObject.h"
#include "EditorCamera.h"
#include "SceneCamera.h"
#include "Shader.h"
#include "Framebuffer.h"
#include "Texture.h"
#include "Utils/UUIDAllocator.h"

namespace Ethereal
{
    class RenderResource;
    static constexpr size_t MaxTextureSlots = 32;
    class RenderScene : public RefCounted {
      public:
        RenderScene() = default;
        void UpdateVisiableMeshNode(Ref<RenderResource>& RenderResource);

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