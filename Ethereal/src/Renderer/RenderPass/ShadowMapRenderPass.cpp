#include "ShadowMapRenderPass.h"

namespace Ethereal
{
    void ShadowMapRenderPass::Init(uint32_t width, uint32_t height) {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::DEPTH};
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

        const auto& staticMeshDrawList = m_DrawLists.StaticMeshDrawList;
        const auto& meshTransformMap = m_DrawLists.MeshTransformMap;

        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
        if (!staticMeshDrawList.empty()) {
            for (auto& [mk, dc] : staticMeshDrawList) {
                Ref<MeshSource> ms = dc.StaticMesh->GetMeshSource();
                Ref<MaterialTable> mt = dc.MaterialTable;
                const auto& meshMaterialTable = dc.StaticMesh->GetMaterials();
                uint32_t materialCount = meshMaterialTable->GetMaterialCount();
                Ref<MaterialAsset> material =
                    mt->HasMaterial(dc.SubmeshIndex) ? mt->GetMaterial(dc.SubmeshIndex) : meshMaterialTable->GetMaterial(dc.SubmeshIndex);

                ms->GetVertexArray()->Bind();
                m_Shader->SetMat4("u_Model", meshTransformMap.at(mk).Transforms[dc.SubmeshIndex].Transform);

                RenderCommand::DrawIndexed(ms->GetVertexArray(), ms->GetVertexArray()->GetIndexBuffer()->GetCount());
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