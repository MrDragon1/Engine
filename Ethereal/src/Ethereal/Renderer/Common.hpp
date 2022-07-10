#pragma once
#include "pch.hpp"

#include "VertexArray.hpp"
#include "Buffer.hpp"
#include "Texture.hpp"
#include "Ethereal/Utils/Math.hpp"

namespace Ethereal
{
    //*************************************************
    //****************** Descriptor *******************
    //*************************************************
    struct GameObjectMeshDesc {
        GameObjectMeshDesc() = default;
        std::string m_filePath;
        bool operator==(const GameObjectMeshDesc& rhs) const { return m_filePath == rhs.m_filePath; }
        size_t getHashValue() const { return std::hash<std::string>{}(m_filePath); }
    };


    struct GameObjectMaterialDesc {
        GameObjectMaterialDesc() = default;
        std::optional<glm::vec4> m_PureColor;  // null if not pure color
        std::string m_base_color_file = "assets\\textures\\default\\default_diffuse.png";
        std::string m_metallic_roughness_file = "assets\\textures\\default\\default_metallic_roughness.png";
        std::string m_normal_file = "assets\\textures\\default\\default_normal.png";
        std::string m_occlusion_file = "assets\\textures\\default\\default_occlusion.png";
        std::string m_emissive_file = "assets\\textures\\default\\default_emissive.png";

        bool operator==(const GameObjectMaterialDesc& rhs) const {
            return m_PureColor == rhs.m_PureColor && m_base_color_file == rhs.m_base_color_file &&
                   m_metallic_roughness_file == rhs.m_metallic_roughness_file && m_normal_file == rhs.m_normal_file &&
                   m_occlusion_file == rhs.m_occlusion_file && m_emissive_file == rhs.m_emissive_file;
        }

        size_t getHashValue() const {
            size_t hash = 0;
            hash_combine(hash, m_base_color_file, m_metallic_roughness_file, m_normal_file, m_occlusion_file, m_emissive_file);
            return hash;
        }
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

    // for hash in unordered_map
    template <>
    struct std::hash<Ethereal::GameObjectMeshDesc> {
        size_t operator()(const Ethereal::GameObjectMeshDesc& rhs) const noexcept { return rhs.getHashValue(); }
    };
    template <>
    struct std::hash<Ethereal::GameObjectMaterialDesc> {
        size_t operator()(const Ethereal::GameObjectMaterialDesc& rhs) const noexcept { return rhs.getHashValue(); }
    };

    //*************************************************
    //******************* CPU Data ********************
    //*************************************************
    struct MeshVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec4 Color;
        glm::vec2 UV;

        // Editor-only
        int EntityID;
    };
    template <typename T>
    class BufferData {
      public:
        size_t m_size{0};
        size_t m_count{0};
        void* m_data{nullptr};

        BufferData() = delete;
        BufferData(size_t count) {
            m_size = count * sizeof(T);
            m_count = count;
            m_data = malloc(m_size);
        }
        ~BufferData() {
            if (m_data) {
                free(m_data);
            }
        }
        bool isValid() const { return m_data != nullptr; }
    };

    struct StaticMeshData {
        Ref<BufferData<MeshVertex>> m_vertex_buffer;
        Ref<BufferData<uint32_t>> m_index_buffer;
        BufferLayout m_layout;
    };

    struct RenderMeshData {
        StaticMeshData m_static_mesh_data;
        // std::shared_ptr<BufferData> m_skeleton_binding_buffer;
    };

    struct RenderEntity {
        size_t m_InstanceID;
        size_t m_MeshAssetID;
        size_t m_MaterialAssetID;

        //* remove this
        GameObjectMeshDesc m_Mesh_Desc;
        GameObjectMaterialDesc m_Material_Desc;
        GameObjectTransformDesc m_Transform_Desc;
    };

    //*************************************************
    //******************* GPU Data ********************
    //*************************************************
    struct GLMesh {
        Ref<VertexArray> m_VAO;
        Ref<VertexBuffer> m_VBO;
        Ref<IndexBuffer> m_IBO;
        uint32_t m_VertexCount;
        uint32_t m_IndexCount;
        BufferLayout m_Layout;
    };

    struct GLMaterial {
        Ref<Texture> m_BaseColorMap;
        Ref<Texture> m_MetallicMap;
        Ref<Texture> m_NormalMap;
        Ref<Texture> m_OcclusionMap;
        Ref<Texture> m_EmissionMap;
        // TODO: uniform data
    };

    struct RenderNode {
        glm::mat4 model_matrix;
        GLMesh* ref_mesh = nullptr;
        GLMaterial* ref_material = nullptr;
        // uint32_t node_id;
    };

}  // namespace Ethereal