#include "SkyboxRenderPass.h"
#include "Ethereal/src/Base/GlobalContext.h"
#include "Core/Renderer/RenderResource.h"
namespace Ethereal
{
    void SkyboxRenderPass::Init(uint32_t width, uint32_t height) {
        m_Cube = RenderResource::Cube;

        m_SkyboxShader = Shader::Create(m_SkyboxShaderPath);
        m_SkyboxShader->Bind();
        m_SkyboxShader->SetInt("u_SkyboxTexture", 0);
    }

    void SkyboxRenderPass::Draw() {
        // ! This will rewrite eneityID attachment in framebuffer
        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LEQUAL);
        m_SkyboxShader->Bind();

        RenderCommand::DrawIndexed(m_Cube->GetMeshSource()->GetVertexArray(), m_Cube->GetMeshSource()->GetIndexBuffer()->GetCount());
        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LESS);
    }

    void SkyboxRenderPass::OnResize(uint32_t width, uint32_t height) {}

}  // namespace Ethereal