#include "RenderPass.hpp"
#include "RenderCommand.hpp"

namespace Ethereal
{
    VisiableNodes RenderPass::m_VisiableNodes;

    void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
        m_ViewProjectionMatrix = glm::mat4(1.0f);

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_UNORM, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32_INTEGER,
                              ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = width;
        fbSpec.Height = height;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_Shader = m_Shader = Shader::Create(m_ShaderPath);
        m_Shader->Bind();

        m_Shader->SetFloat3("albedo", {0.5f, 0.0f, 0.0f});
        m_Shader->SetFloat("ao", 1.0f);

        m_Shader->SetInt("u_BaseColorTexture", 0);
        m_Shader->SetInt("u_MetallicTexture", 1);
        m_Shader->SetInt("u_NormalTexture", 2);
        m_Shader->SetInt("u_OcclusionTexture", 3);
        m_Shader->SetInt("u_EmissionTexture", 4);
        m_Shader->SetInt("u_ShadowMap", 5);
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
            glm::vec3(-10.0f, 10.0f, 10.0f),
            glm::vec3(10.0f, 10.0f, 10.0f),
            glm::vec3(-10.0f, -10.0f, 10.0f),
            glm::vec3(10.0f, -10.0f, 10.0f),
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
                RenderNode.ref_material->m_BaseColorMap->Bind(0);
                RenderNode.ref_material->m_MetallicMap->Bind(1);
                RenderNode.ref_material->m_NormalMap->Bind(2);
                RenderNode.ref_material->m_OcclusionMap->Bind(3);
                RenderNode.ref_material->m_EmissionMap->Bind(4);

                m_Shader->SetFloat("roughness", RenderNode.ref_material->m_Roughness);
                m_Shader->SetFloat("metallic", RenderNode.ref_material->m_Metallic);

                RenderCommand::DrawIndexed(RenderNode.ref_mesh->m_VAO, RenderNode.ref_mesh->m_IndexCount);
            }
        }

        m_Framebuffer->Unbind();
    }

    void MainCameraRenderPass::OnResize(uint32_t width, uint32_t height) { m_Framebuffer->Resize(width, height); }

    int MainCameraRenderPass::GetMousePicking(int x, int y) { return m_Framebuffer->ReadPixel(1, x, y); }

    void ShadowMapRenderPass::Init(uint32_t width, uint32_t height) {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = m_ShadowMapSize;
        fbSpec.Height = m_ShadowMapSize;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_LightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
        CalculateViewProjectionMatrix();

        m_Shader = m_Shader = Shader::Create(m_ShaderPath);
        m_Shader->Bind();
    }

    void ShadowMapRenderPass::Draw() {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0, 0, 0, 0});
        RenderCommand::Clear();
        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::FRONT);

        const auto& visiableRenderNode = *m_VisiableNodes.p_main_camera_visible_mesh_nodes;
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
        if (!visiableRenderNode.empty()) {
            for (auto& RenderNode : visiableRenderNode) {
                RenderNode.ref_mesh->m_VAO->Bind();
                m_Shader->SetMat4("u_Model", RenderNode.model_matrix);
                RenderCommand::DrawIndexed(RenderNode.ref_mesh->m_VAO, RenderNode.ref_mesh->m_IndexCount);
            }
        }

        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::BACK);
        m_Framebuffer->Unbind();
    }

    void ShadowMapRenderPass::OnResize(uint32_t width, uint32_t height) {
        //* framebuffer dont need to resize
        // m_Framebuffer->Resize(width, height);
    }

    void ShadowMapRenderPass::CalculateViewProjectionMatrix() {
        GLfloat near_plane = 0.1f, far_plane = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
        // glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(m_LightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_ViewProjectionMatrix = lightProjection * lightView;
    }
}  // namespace Ethereal