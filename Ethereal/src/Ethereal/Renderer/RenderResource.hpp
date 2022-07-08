#pragma once
#include "pch.hpp"
#include "Common.hpp"

namespace Ethereal
{
    class RenderResource {
      public:
        RenderResource() = default;
        // Do upate data here

        void UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData, RenderMaterialData& RenderMaterialData);
        void UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        void UploadRenderResource(RenderEntity& RenderEntity, RenderMaterialData& RenderMaterialData);

        GLMesh& GetGLMesh(RenderEntity entity);
        GLMaterial& GetGLMaterial(RenderEntity entity);

        RenderMeshData LoadMeshData(GameObjectMeshDesc& mesh_desc, int entityID = -1);
        RenderMaterialData LoadMaterialData(GameObjectMaterialDesc& material_desc);

      private:
        void UploadRenderMesh(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        void UploadRenderMaterial(RenderEntity& RenderEntity, RenderMaterialData& RenderMaterialData);

        void UpdateMesh(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        void UpdateMaterial(RenderEntity& RenderEntity, RenderMaterialData& RenderMaterialData);

      private:
        std::map<size_t, GLMesh> m_Mesh_Map;
        std::map<size_t, GLMaterial> m_Material_Map;
    };
}  // namespace Ethereal