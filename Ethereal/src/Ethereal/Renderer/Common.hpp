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
        std::string m_AlbedoFile = "assets\\textures\\default\\default_diffuse.png";
        std::string m_NormalFile = "assets\\textures\\default\\default_normal.png";
        std::string m_MetallicFile = "assets\\textures\\default\\default_metallic.png";
        std::string m_RoughnessFile = "assets\\textures\\default\\default_roughness.png";
        std::string m_OcclusionFile = "assets\\textures\\default\\default_occlusion.png";
        std::string m_EmissiveFile = "assets\\textures\\default\\default_emissive.png";

        glm::vec3 m_Albedo = glm::vec3(1.0f);
        float m_Metallic = 0.0f;
        float m_Roughness = 0.0f;

        // ! this will cause memory waste in Material_Map, need to improve this one day
        // false -> use texture
        bool b_Albedo = false;
        bool b_Metallic = false;
        bool b_Roughness = false;
        bool b_Normal = false;
        bool b_Occlusion = false;

        bool operator==(const GameObjectMaterialDesc& rhs) const {
            return m_AlbedoFile == rhs.m_AlbedoFile && m_MetallicFile == rhs.m_MetallicFile && m_RoughnessFile == rhs.m_RoughnessFile &&
                   m_NormalFile == rhs.m_NormalFile && m_EmissiveFile == rhs.m_EmissiveFile && m_OcclusionFile == rhs.m_OcclusionFile &&
                   m_Albedo == rhs.m_Albedo && m_Metallic == rhs.m_Metallic && m_Roughness == rhs.m_Roughness && b_Albedo == rhs.b_Albedo &&
                   b_Metallic == rhs.b_Metallic && b_Roughness == rhs.b_Roughness && b_Normal == rhs.b_Normal && b_Occlusion == rhs.b_Occlusion;
        }

        size_t getHashValue() const {
            size_t hash = 0;
            hash_combine(hash, m_AlbedoFile, m_MetallicFile, m_RoughnessFile, m_NormalFile, m_EmissiveFile, m_OcclusionFile,
                         std::to_string(m_Metallic), std::to_string(m_Roughness), std::to_string(b_Albedo), std::to_string(b_Metallic),
                         std::to_string(b_Roughness), std::to_string(b_Normal), std::to_string(b_Occlusion));
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

    //*************************************************
    //******************* CPU Data ********************
    //*************************************************
    struct MeshVertex {
        glm::vec3 Position;
        glm::vec3 Normal;
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
        Ref<Texture> m_AlbedoMap;
        Ref<Texture> m_NormalMap;
        Ref<Texture> m_MetallicMap;
        Ref<Texture> m_RoughnessMap;
        Ref<Texture> m_OcclusionMap;
        Ref<Texture> m_EmissiveMap;

        glm::vec3 m_Albedo = glm::vec3(1.0f);
        float m_Metallic = 0.0f;
        float m_Roughness = 0.0f;

        bool b_Albedo = false;
        bool b_Metallic = false;
        bool b_Roughness = false;
        bool b_Normal = false;
        bool b_Occlusion = false;
    };

    struct RenderNode {
        glm::mat4 model_matrix;
        GLMesh* ref_mesh = nullptr;
        GLMaterial* ref_material = nullptr;
        // uint32_t node_id;
    };

};  // namespace Ethereal

// for hash in unordered_map
template <>
struct std::hash<Ethereal::GameObjectMeshDesc> {
    size_t operator()(const Ethereal::GameObjectMeshDesc& rhs) const noexcept { return rhs.getHashValue(); }
};
template <>
struct std::hash<Ethereal::GameObjectMaterialDesc> {
    size_t operator()(const Ethereal::GameObjectMaterialDesc& rhs) const noexcept { return rhs.getHashValue(); }
};