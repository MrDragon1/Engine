#include "SkyboxRenderPass.hpp"

namespace Ethereal
{
    void SkyboxRenderPass::Init(uint32_t width, uint32_t height) {
        m_SkyboxShader = Shader::Create(m_SkyboxShaderPath);
        m_SkyboxShader->Bind();
        m_SkyboxShader->SetInt("u_SkyboxTexture", 0);
    }

    void SkyboxRenderPass::Draw() {
        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LEQUAL);
        m_SkyboxShader->Bind();
        m_SkyboxShader->SetMat4("u_Projection", m_SkyboxProjection);
        m_SkyboxShader->SetMat4("u_View", m_SkyboxView);

        RenderCommand::DrawIndexed(m_Cube.m_VAO, m_Cube.m_IndexCount);

        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LESS);
    }

    void SkyboxRenderPass::OnResize(uint32_t width, uint32_t height) {}

}