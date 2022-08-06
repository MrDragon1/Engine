#pragma once
#include "Core/Base/AABB.h"
#include "Core/Timestep.h"
#include "Asset/Asset.h"
#include "Buffer.h"
#include "Shader.h"
#include "Texture.h"
#include "MaterialAsset.h"
#include "VertexArray.h"

#include <vector>
#include <glm/glm.hpp>

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp
{
    class Importer;
}
namespace Ethereal
{
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec3 Tangent;
        glm::vec3 Binormal;
        glm::vec2 Texcoord;
    };

    struct Index {
        uint32_t V1, V2, V3;
    };

    static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

    struct Triangle {
        Vertex V0, V1, V2;

        Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2) : V0(v0), V1(v1), V2(v2) {}
    };

    class Submesh {
      public:
        uint32_t BaseVertex;
        uint32_t BaseIndex;
        uint32_t MaterialIndex;
        uint32_t IndexCount;
        uint32_t VertexCount;

        glm::mat4 Transform{1.0f};  // World transform
        glm::mat4 LocalTransform{1.0f};
        AABB BoundingBox;

        std::string NodeName, MeshName;
    };

    //
    // MeshSource is a representation of an actual asset file on disk
    // Meshes are created from MeshSource
    //
    class MeshSource : public Asset {
      public:
        MeshSource(const std::string& filename);
        MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const glm::mat4& transform);
        MeshSource(const std::vector<Vertex>& vertices, const std::vector<Index>& indices, const std::vector<Submesh>& submeshes);
        virtual ~MeshSource();

        std::vector<Submesh>& GetSubmeshes() { return m_Submeshes; }
        const std::vector<Submesh>& GetSubmeshes() const { return m_Submeshes; }

        const std::vector<Vertex>& GetVertices() const { return m_StaticVertices; }
        const std::vector<Index>& GetIndices() const { return m_Indices; }

        std::vector<Ref<Material>>& GetMaterials() { return m_Materials; }
        const std::vector<Ref<Material>>& GetMaterials() const { return m_Materials; }
        const std::string& GetFilePath() const { return m_FilePath; }

        Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
        Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
        Ref<VertexArray> GetVertexArray() { return m_VertexArray; }
        const BufferLayout& GetVertexBufferLayout() const { return m_VertexBufferLayout; }

        static AssetType GetStaticType() { return AssetType::MeshSource; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

        const AABB& GetBoundingBox() const { return m_BoundingBox; }

      private:
        void TraverseNodes(aiNode* node, const glm::mat4& parentTransform = glm::mat4(1.0f), uint32_t level = 0);

      private:
        std::unique_ptr<Assimp::Importer> m_Importer;
        const aiScene* m_Scene;
        std::string m_FilePath;

        glm::mat4 m_InverseTransform;

        Ref<VertexArray> m_VertexArray;
        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;
        BufferLayout m_VertexBufferLayout;

        std::vector<Submesh> m_Submeshes;
        AABB m_BoundingBox;

        std::vector<Vertex> m_StaticVertices;
        std::vector<Index> m_Indices;
        std::unordered_map<aiNode*, std::vector<uint32_t>> m_NodeMap;

        // Materials
        std::vector<Ref<Material>> m_Materials;
    };

    // Static Mesh - no skeletal animation, flattened hierarchy
    class StaticMesh : public Asset {
      public:
        explicit StaticMesh(Ref<MeshSource> meshSource);
        StaticMesh(Ref<MeshSource> meshSource, const std::vector<uint32_t>& submeshes);
        StaticMesh(const Ref<StaticMesh>& other);
        virtual ~StaticMesh();

        std::vector<uint32_t>& GetSubmeshes() { return m_Submeshes; }
        const std::vector<uint32_t>& GetSubmeshes() const { return m_Submeshes; }

        // Pass in an empty vector to set ALL submeshes for MeshSource
        void SetSubmeshes(const std::vector<uint32_t>& submeshes);

        Ref<MeshSource> GetMeshSource() { return m_MeshSource; }
        Ref<MeshSource> GetMeshSource() const { return m_MeshSource; }
        void SetMeshAsset(Ref<MeshSource> meshAsset) { m_MeshSource = meshAsset; }

        Ref<MaterialTable> GetMaterials() const { return m_Materials; }

        static AssetType GetStaticType() { return AssetType::StaticMesh; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

      private:
        Ref<MeshSource> m_MeshSource;
        std::vector<uint32_t> m_Submeshes;

        // Materials
        Ref<MaterialTable> m_Materials;

        friend class Scene;
        friend class Renderer;
        friend class SceneHierarchyPanel;
    };

}  // namespace Ethereal