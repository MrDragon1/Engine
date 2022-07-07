#pragma once
#include "pch.hpp"

#include "VertexArray.hpp"
#include "Buffer.hpp"
namespace Ethereal
{
    struct GameObjectMeshDesc {
        std::string m_filePath;
    };

    struct GameObjectTransformDesc {
        glm::vec3 Translation = glm::vec3(0.0f);
        glm::vec3 Rotation = glm::vec3(0.0f);
        glm::vec3 Scale = glm::vec3(1.0f);

        glm::mat4 GetTransform() {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));
            return glm::translate(glm::mat4(1.0f), Translation) * rotation * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct QuadVertex {
        glm::vec3 Position;
        glm::vec4 Color;
        glm::vec2 TexCoord;
        // TODO: texid
        float TexIndex;
        float TilingFactor;

        // Editor-only
        int EntityID;
    };
    class BufferData {
      public:
        size_t m_size{0};
        void* m_data{nullptr};

        BufferData() = delete;
        BufferData(size_t size) {
            m_size = size;
            m_data = malloc(size);
        }
        ~BufferData() {
            if (m_data) {
                free(m_data);
            }
        }
        bool isValid() const { return m_data != nullptr; }
    };

    struct StaticMeshData {
        Ref<BufferData> m_vertex_buffer;
        Ref<BufferData> m_index_buffer;
        BufferLayout m_layout;
    };

    struct RenderMeshData {
        StaticMeshData m_static_mesh_data;
        // std::shared_ptr<BufferData> m_skeleton_binding_buffer;
    };

    struct RenderEntity {
        // size_t m_InstanceID;
        size_t m_AssetID;

        GameObjectMeshDesc m_Mesh_Desc;
        GameObjectTransformDesc m_Transform_Desc;
        RenderMeshData m_MeshData;  // Temporary
    };

    // GPU Data
    struct GLMesh {
        Ref<VertexArray> m_VAO;
        Ref<VertexBuffer> m_VBO;
        Ref<IndexBuffer> m_IBO;
        uint32_t m_VertexCount;
        uint32_t m_IndexCount;
        BufferLayout m_Layout;
    };

    struct RenderMeshNode {
        glm::mat4 model_matrix;
        GLMesh* ref_mesh = nullptr;
        // uint32_t node_id;
    };

}  // namespace Ethereal