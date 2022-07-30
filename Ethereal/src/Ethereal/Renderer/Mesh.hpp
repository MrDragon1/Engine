#pragma once
#include "Ethereal/Core/Base/AABB.hpp"
#include "Ethereal/Core/Timestep.hpp"
#include "Ethereal/Asset/Asset.hpp"

#include <vector>
#include <glm/glm.hpp>

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
    };

    // Static Mesh - no skeletal animation, flattened hierarchy
    class StaticMesh : public Asset {
      public:
    };

}  // namespace Ethereal