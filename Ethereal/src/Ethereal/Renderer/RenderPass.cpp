#include "RenderPass.hpp"
#include "RenderCommand.hpp"

namespace Ethereal
{
    VisiableNodes RenderPass::m_VisiableNodes;

    void MainCameraRenderPass::Init() {
        m_ViewProjectionMatrix = glm::mat4(1.0f);
        m_Shader = m_Shader = Shader::Create(m_ShaderPath);

        m_Shader->SetInt("u_BaseColorTexture", 0);
        m_Shader->SetInt("u_MetallicTexture", 1);
        m_Shader->SetInt("u_NormalTexture", 2);
        m_Shader->SetInt("u_OcclusionTexture", 3);
        m_Shader->SetInt("u_EmissionTexture", 4);
    }


    void MainCameraRenderPass::Draw()
    {
        const auto& visiableRenderNode = *m_VisiableNodes.p_main_camera_visible_mesh_nodes;
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
        if (visiableRenderNode.empty()) return;
        for (auto& RenderNode : visiableRenderNode) {
            RenderNode.ref_mesh->m_VAO->Bind();
            m_Shader->SetMat4("u_Model", RenderNode.model_matrix);
            RenderNode.ref_material->m_BaseColorMap->Bind(0);
            RenderNode.ref_material->m_MetallicMap->Bind(1);
            RenderNode.ref_material->m_NormalMap->Bind(2);
            RenderNode.ref_material->m_OcclusionMap->Bind(3);
            RenderNode.ref_material->m_EmissionMap->Bind(4);
            RenderCommand::DrawIndexed(RenderNode.ref_mesh->m_VAO, RenderNode.ref_mesh->m_IndexCount);
        }
    }
}  // namespace Ethereal