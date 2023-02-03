#include "SkyboxRenderPass.h"
#include "Ethereal/src/Base/GlobalContext.h"
#include "Core/Renderer/RenderResource.h"
namespace Ethereal
{
    void SkyboxRenderPass::Init(uint32_t width, uint32_t height) {
        m_Cube = RenderResource::Cube;

        ETHEREAL_SHADER_PACK SkyboxShaderPack = {
            {ETHEREAL_SHADER_TYPE::VERTEX, SKYBOX_VERT},
            {ETHEREAL_SHADER_TYPE::FRAGMENT, SKYBOX_FRAG},
        };
        m_SkyboxShader = GlobalContext::GetShaderLibrary().Get("SKYBOX");
        m_SkyboxShader->Bind();
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