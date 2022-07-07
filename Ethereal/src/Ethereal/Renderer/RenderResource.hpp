#pragma once
#include "pch.hpp"
#include "Common.hpp"

namespace Ethereal
{
    class RenderResource {
      public:
        RenderResource() = default;
        // Do upate data here

        void UploadRenderResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        void UpdateResource(RenderEntity& RenderEntity, RenderMeshData& RenderMeshData);
        GLMesh& GetGLMesh(RenderEntity entity);

      private:
        std::map<size_t, GLMesh> m_Mesh_Map;
    };
}  // namespace Ethereal