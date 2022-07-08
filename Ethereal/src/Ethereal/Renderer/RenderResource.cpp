#include "RenderResource.hpp"
#include "Ethereal/Utils/AssetLoader.hpp"
namespace Ethereal
{

    void RenderResource::UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData, RenderMaterialData& RenderMaterialData) {
        UploadRenderMesh(RenderEntity, RenderMeshData);
        UploadRenderMaterial(RenderEntity, RenderMaterialData);
    }

    void RenderResource::UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData) {
        UploadRenderMesh(RenderEntity, RenderMeshData);
    }

    void RenderResource::UploadRenderResource(RenderEntity& RenderEntity, RenderMaterialData& RenderMaterialData) {
        UploadRenderMaterial(RenderEntity, RenderMaterialData);
    }

    void RenderResource::UploadRenderMesh(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData) {
        size_t meshAssetId = RenderEntity.m_MeshAssetID;
        auto it = m_Mesh_Map.find(meshAssetId);
        if (it != m_Mesh_Map.end()) {
            // TODO: move to dirty data update func
            UpdateMesh(RenderEntity, RenderMeshData);
            // Alread exist
            // ET_CORE_INFO("Already exist {0}", assetid);
        } else {
            GLMesh temp_mesh;
            temp_mesh.m_Layout = RenderMeshData.m_static_mesh_data.m_layout;
            ET_CORE_ASSERT(RenderMeshData.m_static_mesh_data.m_vertex_buffer->isValid(),"m_vertex_buffer is null");
            ET_CORE_ASSERT(RenderMeshData.m_static_mesh_data.m_index_buffer->isValid(),"m_index_buffer is null");
            temp_mesh.m_VertexCount = RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_count;
            temp_mesh.m_IndexCount = RenderMeshData.m_static_mesh_data.m_index_buffer->m_count;

            temp_mesh.m_VAO = VertexArray::Create();
            temp_mesh.m_VBO = VertexBuffer::Create(temp_mesh.m_VertexCount * sizeof(MeshVertex));
            temp_mesh.m_VBO->SetLayout(temp_mesh.m_Layout);
            temp_mesh.m_VBO->SetData(RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_data, temp_mesh.m_VertexCount * sizeof(MeshVertex));
            temp_mesh.m_VAO->AddVertexBuffer(temp_mesh.m_VBO);

            temp_mesh.m_IBO = IndexBuffer::Create((uint32_t*)RenderMeshData.m_static_mesh_data.m_index_buffer->m_data, temp_mesh.m_IndexCount);
            temp_mesh.m_VAO->SetIndexBuffer(temp_mesh.m_IBO);

            auto res = m_Mesh_Map.insert(std::make_pair(meshAssetId, std::move(temp_mesh)));
            ET_CORE_ASSERT(res.second, "Failed to insert mesh into map");
        }
    }

    void RenderResource::UploadRenderMaterial(RenderEntity& RenderEntity, RenderMaterialData& RenderMaterialData) {
        size_t materialAssetId = RenderEntity.m_MaterialAssetID;
        auto it = m_Material_Map.find(materialAssetId);
        if (it != m_Material_Map.end()) {
            // TODO: move to dirty data update func
            UpdateMaterial(RenderEntity, RenderMaterialData);
            // Alread exist
            // ET_CORE_INFO("Already exist {0}", assetid);
        } else {
            GLMaterial temp_material;
            ET_CORE_ASSERT(RenderMaterialData.m_BaseColorData, "Material must have BaseColorData");
            temp_material.m_BaseColorMap = Texture2D::Create(RenderMaterialData.m_BaseColorData);
            
            if (RenderMaterialData.m_EmissionData) temp_material.m_EmissionMap = Texture2D::Create(RenderMaterialData.m_EmissionData);
            if (RenderMaterialData.m_MetallicData) temp_material.m_MetallicMap = Texture2D::Create(RenderMaterialData.m_MetallicData);
            if (RenderMaterialData.m_OcclusionData) temp_material.m_OcclusionMap = Texture2D::Create(RenderMaterialData.m_OcclusionData);
            if (RenderMaterialData.m_NormalData) temp_material.m_NormalMap = Texture2D::Create(RenderMaterialData.m_NormalData);

            auto res = m_Material_Map.insert(std::make_pair(materialAssetId, std::move(temp_material)));
            ET_CORE_ASSERT(res.second, "Failed to insert material into map");
        }
    }

    void RenderResource::UpdateMesh(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData) {
        size_t meshAssetId = RenderEntity.m_MeshAssetID;
        auto it = m_Mesh_Map.find(meshAssetId);
        if (it != m_Mesh_Map.end()) {
            GLMesh& mesh = it->second;
            ET_CORE_ASSERT(mesh.m_VertexCount == RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_count, "Vertex count mismatch");
            ET_CORE_ASSERT(mesh.m_IndexCount == RenderMeshData.m_static_mesh_data.m_index_buffer->m_count, "Vertex count mismatch");

            mesh.m_VBO->SetData(RenderMeshData.m_static_mesh_data.m_vertex_buffer->m_data, mesh.m_VertexCount * sizeof(MeshVertex));
            mesh.m_VAO->SetIndexBuffer(mesh.m_IBO);
        } else {
            ET_CORE_ASSERT("Failed to find mesh in map");
        }
    }

    void RenderResource::UpdateMaterial(RenderEntity& RenderEntity, RenderMaterialData& RenderMaterialData) {
        // Do nothing
    }

    GLMesh& RenderResource::GetGLMesh(RenderEntity entity) {
        size_t meshAssetId = entity.m_MeshAssetID;

        auto it = m_Mesh_Map.find(meshAssetId);
        if (it != m_Mesh_Map.end()) {
            return it->second;
        } else {
            throw std::runtime_error("failed to get entity mesh");
        }
    }
    
    GLMaterial& RenderResource::GetGLMaterial(RenderEntity entity){
        size_t materialAssetId = entity.m_MaterialAssetID;

        auto it = m_Material_Map.find(materialAssetId);
        if (it != m_Material_Map.end()) {
            return it->second;
        } else {
            throw std::runtime_error("failed to get entity material");
        }
    }
    
    //TODO: use another way to mouse picking and remove entityID
    RenderMeshData RenderResource::LoadMeshData(GameObjectMeshDesc& mesh_desc, int entityID)
    {
        RenderMeshData meshData;
        meshData.m_static_mesh_data.m_layout = {
            {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"}, {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float2, "a_TexCoord"}, {ShaderDataType::Int, "a_EntityID"},
        };
        ObjLoader::Load(mesh_desc, meshData);
        return meshData;
    } 
    
    RenderMaterialData RenderResource::LoadMaterialData(GameObjectMaterialDesc& material_desc)
    {
        RenderMaterialData renderMaterialData;
        TextureLoader::Load(material_desc, renderMaterialData);
        return renderMaterialData;
    }
    
}  // namespace Ethereal