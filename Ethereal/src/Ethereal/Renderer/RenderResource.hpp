#pragma once
#include "pch.hpp"
#include "Common.hpp"

namespace Ethereal
{
    class RenderResource {
      public:
        RenderResource() = default;
        // Do upate data here

        void UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData, GameObjectMaterialDesc& GameObjectMaterialDesc);
        void UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        void UploadRenderResource(RenderEntity& RenderEntity, GameObjectMaterialDesc& GameObjectMaterialDesc);

        GLMesh& GetGLMesh(RenderEntity entity);
        GLMaterial& GetGLMaterial(RenderEntity entity);

        RenderMeshData LoadMeshData(GameObjectMeshDesc& mesh_desc);
        void LoadMaterialData(GameObjectMaterialDesc& material_desc);

      private:
        void UploadRenderMesh(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        void UploadRenderMaterial(RenderEntity& RenderEntity, GameObjectMaterialDesc& GameObjectMaterialDesc);

        void UpdateMesh(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        void UpdateMaterial(RenderEntity& RenderEntity, GameObjectMaterialDesc& GameObjectMaterialDesc);

      private:
        std::map<size_t, GLMesh> m_Mesh_Map;
        std::map<size_t, GLMaterial> m_Material_Map;
    };
}  // namespace Ethereal