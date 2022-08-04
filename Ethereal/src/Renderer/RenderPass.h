#pragma once
#include "Common.h"
#include "Shader.h"
#include "FrameBuffer.h"
#include "RenderCommand.h"
#include "Mesh.h"
#include "Material.h"
#include "Core/Base/Ref.h"

namespace Ethereal
{
    struct TransformVertexData {
        glm::mat4 Transform;
    };
    struct StaticDrawCommand {
        Ref<StaticMesh> StaticMesh;
        uint32_t SubmeshIndex;
        Ref<MaterialTable> MaterialTable;
        Ref<Material> OverrideMaterial;

        uint32_t InstanceCount = 0;
        uint32_t InstanceOffset = 0;
    };

    struct MeshKey {
        AssetHandle MeshHandle;
        AssetHandle MaterialHandle;
        uint32_t SubmeshIndex;
        bool IsSelected;
        uint32_t EntityID = -1;

        MeshKey(AssetHandle meshHandle, AssetHandle materialHandle, uint32_t submeshIndex, bool isSelected, uint32_t id = -1)
            : MeshHandle(meshHandle), MaterialHandle(materialHandle), SubmeshIndex(submeshIndex), IsSelected(isSelected), EntityID(id) {}

        bool operator<(const MeshKey& other) const {
            if (MeshHandle < other.MeshHandle) return true;

            if (MeshHandle > other.MeshHandle) return false;

            if (SubmeshIndex < other.SubmeshIndex) return true;

            if (SubmeshIndex > other.SubmeshIndex) return false;

            if (MaterialHandle < other.MaterialHandle) return true;

            if (MaterialHandle > other.MaterialHandle) return false;

            return IsSelected < other.IsSelected;
        }
    };

    struct TransformMapData {
        std::vector<TransformVertexData> Transforms;
        uint32_t TransformOffset = 0;
    };

    struct DrawLists {
        std::map<MeshKey, TransformMapData> MeshTransformMap;
        std::map<MeshKey, StaticDrawCommand> StaticMeshDrawList;
    };

    class RenderPass : public RefCounted {
      public:
        RenderPass() = default;
        virtual ~RenderPass() = default;
        virtual void Init(uint32_t width, uint32_t height) = 0;
        virtual void Draw() = 0;
        virtual void OnResize(uint32_t width, uint32_t height) = 0;

        static DrawLists m_DrawLists;

        friend class RenderSystem;
    };

}  // namespace Ethereal