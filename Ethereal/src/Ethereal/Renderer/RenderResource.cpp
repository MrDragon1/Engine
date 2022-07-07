#include "RenderResource.hpp"

namespace Ethereal
{
    // TODO: Move to Platform/
    void RenderResource::UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData) {
        size_t assetid = RenderEntity.m_AssetID;
        auto it = m_Mesh_Map.find(assetid);
        if (it != m_Mesh_Map.end()) {
            UpdateResource(RenderEntity, RenderMeshData);
            // Alread exist
            // ET_CORE_INFO("Already exist {0}", assetid);
        } else {
            GLMesh temp_mesh;
            temp_mesh.m_Layout = RenderMeshData.m_static_mesh_data.m_layout;
            temp_mesh.m_VertexCount = RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_size / sizeof(MeshVertex);
            temp_mesh.m_IndexCount = RenderMeshData.m_static_mesh_data.m_index_buffer->m_size / sizeof(uint32_t);

            temp_mesh.m_VAO = VertexArray::Create();
            temp_mesh.m_VBO = VertexBuffer::Create(temp_mesh.m_VertexCount * sizeof(MeshVertex));
            temp_mesh.m_VBO->SetLayout(temp_mesh.m_Layout);
            temp_mesh.m_VBO->SetData(RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_data, temp_mesh.m_VertexCount * sizeof(MeshVertex));
            temp_mesh.m_VAO->AddVertexBuffer(temp_mesh.m_VBO);

            temp_mesh.m_IBO = IndexBuffer::Create((uint32_t*)RenderMeshData.m_static_mesh_data.m_index_buffer->m_data, temp_mesh.m_IndexCount);
            temp_mesh.m_VAO->SetIndexBuffer(temp_mesh.m_IBO);

            auto res = m_Mesh_Map.insert(std::make_pair(assetid, std::move(temp_mesh)));
            ET_CORE_ASSERT(res.second, "Failed to insert mesh into map");
        }
    }

    void RenderResource::UpdateResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData) {
        size_t assetid = RenderEntity.m_AssetID;
        auto it = m_Mesh_Map.find(assetid);
        if (it != m_Mesh_Map.end()) {
            GLMesh& mesh = it->second;
            ET_CORE_ASSERT(mesh.m_VertexCount == RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_size / sizeof(MeshVertex),
                           "Vertex count mismatch");
            ET_CORE_ASSERT(mesh.m_IndexCount == RenderMeshData.m_static_mesh_data.m_index_buffer->m_size / sizeof(uint32_t), "Vertex count mismatch");

            mesh.m_VBO->SetData(RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_data, mesh.m_VertexCount * sizeof(MeshVertex));
            mesh.m_VAO->SetIndexBuffer(mesh.m_IBO);
        } else {
            ET_CORE_ASSERT("Failed to find mesh in map");
        }
    }

    GLMesh& RenderResource::GetGLMesh(RenderEntity entity) {
        size_t assetid = entity.m_AssetID;

        auto it = m_Mesh_Map.find(assetid);
        if (it != m_Mesh_Map.end()) {
            return it->second;
        } else {
            throw std::runtime_error("failed to get entity mesh");
        }
    }
}  // namespace Ethereal