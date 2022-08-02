#pragma once
#include "Ethereal/Core/Base/AABB.hpp"
#include "Ethereal/Core/Timestep.hpp"
#include "Ethereal/Asset/Asset.hpp"
#include "Buffer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"

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
    class Material; //TODO! implement this
    
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

		Ref<Shader> GetMeshShader() { return m_MeshShader; }
		std::vector<Ref<Material>>& GetMaterials() { return m_Materials; }
		const std::vector<Ref<Material>>& GetMaterials() const { return m_Materials; }
		const std::vector<Ref<Texture2D>>& GetTextures() const { return m_Textures; }
		const std::string& GetFilePath() const { return m_FilePath; }

		const std::vector<Triangle> GetTriangleCache(uint32_t index) const { return m_TriangleCache.at(index); }

		Ref<VertexBuffer> GetVertexBuffer() { return m_VertexBuffer; }
		Ref<IndexBuffer> GetIndexBuffer() { return m_IndexBuffer; }
		const BufferLayout& GetVertexBufferLayout() const { return m_VertexBufferLayout; }

		static AssetType GetStaticType() { return AssetType::MeshSource; }
		virtual AssetType GetAssetType() const override { return GetStaticType(); }

		const AABB& GetBoundingBox() const { return m_BoundingBox; }

      private:
        std::vector<Submesh> m_Submeshes;

        std::unique_ptr<Assimp::Importer> m_Importer;

        glm::mat4 m_InverseTransform;

        Ref<VertexBuffer> m_VertexBuffer;
        Ref<IndexBuffer> m_IndexBuffer;
        BufferLayout m_VertexBufferLayout;

        std::vector<Vertex> m_StaticVertices;
        std::vector<Index> m_Indices;
        std::unordered_map<std::string, uint32_t> m_BoneMapping;
        std::unordered_map<aiNode*, std::vector<uint32_t>> m_NodeMap;
        const aiScene* m_Scene;

        // Materials
        Ref<Shader> m_MeshShader;
        std::vector<Ref<Texture2D>> m_Textures;
        std::vector<Ref<Texture2D>> m_NormalMaps;
        std::vector<Ref<Material>> m_Materials;

        std::unordered_map<uint32_t, std::vector<Triangle>> m_TriangleCache;

        AABB m_BoundingBox;

        std::string m_FilePath;
    };

    // Static Mesh - no skeletal animation, flattened hierarchy
    class StaticMesh : public Asset {
      public:
    };

}  // namespace Ethereal