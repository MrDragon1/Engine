#include "MainCameraRenderPass.h"

namespace Ethereal
{
    void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
        m_ViewProjectionMatrix = glm::mat4(1.0f);

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_UNORM, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32_INTEGER,
                              ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = width;
        fbSpec.Height = height;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_Shader = Shader::Create(m_ShaderPath);
        m_Shader->Bind();

        m_Shader->SetFloat3("u_Albedo", {0.5f, 0.0f, 0.0f});
        m_Shader->SetFloat("u_Occlusion", 1.0f);

        m_Shader->SetInt("u_AlbedoMap", 0);
        m_Shader->SetInt("u_NormalMap", 1);
        m_Shader->SetInt("u_MetallicMap", 2);
        m_Shader->SetInt("u_RoughnessMap", 3);
        m_Shader->SetInt("u_OcclusionMap", 4);
        m_Shader->SetInt("u_EmissiveMap", 5);

        // IBL 
        m_Shader->SetInt("u_IrradianceMap", 6);
        m_Shader->SetInt("u_PrefilterMap", 7);
        m_Shader->SetInt("u_BRDFLUT", 8);
    }

    void MainCameraRenderPass::Draw() {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        const auto& visiableRenderNode = *m_VisiableNodes.p_main_camera_visible_mesh_nodes;
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
        m_Shader->SetMat4("u_LightSpaceMatrix", m_LightSpaceMatrix);
        m_Shader->SetFloat3("camPos", m_CameraPosition);

        // lights
        // ------
        glm::vec3 lightPositions[] = {
            glm::vec3(-10.0f, 20.0f, 10.0f),
            glm::vec3(10.0f, 20.0f, 10.0f),
            glm::vec3(-10.0f, 0.0f, 10.0f),
            glm::vec3(10.0f, 0.0f, 10.0f),
        };
        glm::vec3 lightColors[] = {glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
                                   glm::vec3(300.0f, 300.0f, 300.0f)};
        for (int i = 0; i < 4; i++) {
            m_Shader->SetFloat3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            m_Shader->SetFloat3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        if (!visiableRenderNode.empty()) {
            for (auto& RenderNode : visiableRenderNode) {
                RenderNode.ref_mesh->m_VAO->Bind();
                m_Shader->SetMat4("u_Model", RenderNode.model_matrix);
                m_Shader->SetInt("u_EntityID", RenderNode.EntityID);
                
                RenderNode.ref_material->m_AlbedoMap->Bind(0);
                RenderNode.ref_material->m_NormalMap->Bind(1);
                RenderNode.ref_material->m_MetallicMap->Bind(2);
                RenderNode.ref_material->m_RoughnessMap->Bind(3);
                RenderNode.ref_material->m_OcclusionMap->Bind(4);
                RenderNode.ref_material->m_EmissiveMap->Bind(5);

                m_Shader->SetFloat3("u_Albedo", RenderNode.ref_material->m_Albedo);
                m_Shader->SetFloat("u_Roughness", RenderNode.ref_material->m_Roughness);
                m_Shader->SetFloat("u_Metallic", RenderNode.ref_material->m_Metallic);

                m_Shader->SetInt("u_UseAlbedoMap", RenderNode.ref_material->b_Albedo);
                m_Shader->SetInt("u_UseNormalMap", RenderNode.ref_material->b_Normal);
                m_Shader->SetInt("u_UseMetallicMap", RenderNode.ref_material->b_Metallic);
                m_Shader->SetInt("u_UseRoughnessMap", RenderNode.ref_material->b_Roughness);
                m_Shader->SetInt("u_UseOcclusionMap", RenderNode.ref_material->b_Occlusion);

                RenderCommand::DrawIndexed(RenderNode.ref_mesh->m_VAO, RenderNode.ref_mesh->m_IndexCount);
            }
        }

        m_Framebuffer->Unbind();
    }

    void MainCameraRenderPass::OnResize(uint32_t width, uint32_t height) { m_Framebuffer->Resize(width, height); }

    int MainCameraRenderPass::GetMousePicking(int x, int y) { return m_Framebuffer->ReadPixel(1, x, y); }
}  // namespace Ethereal
