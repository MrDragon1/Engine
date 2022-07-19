#include "RenderResource.hpp"
#include "Ethereal/Utils/AssetManager.hpp"
#include "Ethereal/Utils/AssetLoader.hpp" //Temporary
namespace Ethereal
{

    void RenderResource::UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData, GameObjectMaterialDesc& GameObjectMaterialDesc) {
        UploadRenderMesh(RenderEntity, RenderMeshData);
        UploadRenderMaterial(RenderEntity, GameObjectMaterialDesc);
    }

    void RenderResource::UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData) {
        UploadRenderMesh(RenderEntity, RenderMeshData);
    }

    void RenderResource::UploadRenderResource(RenderEntity& RenderEntity, GameObjectMaterialDesc& GameObjectMaterialDesc) {
        UploadRenderMaterial(RenderEntity, GameObjectMaterialDesc);
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

    void RenderResource::UploadRenderMaterial(RenderEntity& RenderEntity, GameObjectMaterialDesc& GameObjectMaterialDesc) {
        size_t materialAssetId = RenderEntity.m_MaterialAssetID;
        auto it = m_Material_Map.find(materialAssetId);
        if (it != m_Material_Map.end()) {
            // TODO: move to dirty data update func
            UpdateMaterial(RenderEntity, GameObjectMaterialDesc);
            // Alread exist
            // ET_CORE_INFO("Already exist {0}", assetid);
        } else {
            GLMaterial temp_material;
            temp_material.m_AlbedoMap = TextureManager::GetTexture(GameObjectMaterialDesc.m_AlbedoFile);
            temp_material.m_EmissiveMap = TextureManager::GetTexture(GameObjectMaterialDesc.m_EmissiveFile);
            temp_material.m_RoughnessMap = TextureManager::GetTexture(GameObjectMaterialDesc.m_RoughnessFile);
            temp_material.m_MetallicMap = TextureManager::GetTexture(GameObjectMaterialDesc.m_MetallicFile);
            temp_material.m_OcclusionMap = TextureManager::GetTexture(GameObjectMaterialDesc.m_OcclusionFile);
            temp_material.m_NormalMap = TextureManager::GetTexture(GameObjectMaterialDesc.m_NormalFile);
            
            temp_material.m_Albedo = GameObjectMaterialDesc.m_Albedo;
            temp_material.m_Metallic = GameObjectMaterialDesc.m_Metallic;
            temp_material.m_Roughness = GameObjectMaterialDesc.m_Roughness;
            
            temp_material.b_Albedo = GameObjectMaterialDesc.b_Albedo;
            temp_material.b_Normal = GameObjectMaterialDesc.b_Normal;
            temp_material.b_Metallic = GameObjectMaterialDesc.b_Metallic;
            temp_material.b_Roughness = GameObjectMaterialDesc.b_Roughness;
            temp_material.b_Occlusion = GameObjectMaterialDesc.b_Occlusion;

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

    void RenderResource::UpdateMaterial(RenderEntity& RenderEntity, GameObjectMaterialDesc& GameObjectMaterialDesc) {
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
            {ShaderDataType::Float3, "a_Position"}, {ShaderDataType::Float3, "a_Normal"},
            {ShaderDataType::Float2, "a_TexCoord"}, {ShaderDataType::Int, "a_EntityID"},
        };
        ObjLoader::Load(mesh_desc, meshData, entityID);
        return meshData;
    } 
    
    void RenderResource::LoadMaterialData(GameObjectMaterialDesc& material_desc)
    {
        TextureManager::AddTexture(material_desc.m_AlbedoFile);
        TextureManager::AddTexture(material_desc.m_MetallicFile);
        TextureManager::AddTexture(material_desc.m_RoughnessFile);
        TextureManager::AddTexture(material_desc.m_NormalFile);
        TextureManager::AddTexture(material_desc.m_OcclusionFile);
        TextureManager::AddTexture(material_desc.m_EmissiveFile);
    }
    
}  // namespace Ethereal