#include <Core/GlobalContext.h>
#include "BloomRenderPass.h"

namespace Ethereal
{
    void BloomRenderPass::Init(uint32_t width, uint32_t height) {
        m_Quad = GlobalContext::GetRenderSystem().GetQuadStaticMesh();

        FramebufferSpecification fbSpec;
        // Input Image, BrightArea Image (Blur)
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_PLACEHOLDER, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = width;
        fbSpec.Height = height;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_Shader_Bright = Shader::Create("assets/shaders/bloom/bright.glsl");
        m_Shader_Bright->Bind();
        m_Shader_Blur = Shader::Create("assets/shaders/bloom/blur.glsl");
        m_Shader_Blur->Bind();
        m_Shader_Merge = Shader::Create("assets/shaders/bloom/merge.glsl");
        m_Shader_Merge->Bind();

        Invalidate(width, height);
    }

    void BloomRenderPass::Draw() {
        if (m_MainImage == nullptr) {
            ET_CORE_WARN("BloomRenderPass::Draw() - MainImage is nullptr");
            return;
        }

        m_Framebuffer->Bind();

        // Draw BrightArea Image
        m_Shader_Bright->Bind();
        m_Shader_Bright->SetInt("u_MainImage", 0);
        m_Shader_Bright->SetFloat("u_Threshold", 0.6f);
        m_MainImage->Bind(0);
        m_BrightAreaImage->BindToFramebuffer(0);
        RenderCommand::Clear();
        RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());

        // Blur Image
        m_Shader_Blur->Bind();
        m_Shader_Blur->SetInt("u_BrightImage", 0);
        bool horizontal = true, first_iteration = true;
        for (int i = 0; i < 10; i++) {
            m_Shader_Blur->SetInt("u_Horizontal", horizontal);
            // Set blur parameters here
            m_BlurImage[horizontal ? 0 : 1]->BindToFramebuffer(0);
            first_iteration ? m_BrightAreaImage->Bind(0) : m_BlurImage[!horizontal ? 0 : 1]->Bind(0);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());
            horizontal = !horizontal;
            first_iteration = false;
        }

        // Merge Image
        m_Shader_Merge->Bind();
        m_Shader_Merge->SetInt("u_MainImage", 0);
        m_MainImage->Bind(0);
        m_Shader_Merge->SetInt("u_BlurImage", 1);
        m_BlurImage[!horizontal ? 0 : 1]->Bind(1);
        m_BloomImage->BindToFramebuffer(0);
        RenderCommand::Clear();
        RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());

        m_Framebuffer->Unbind();
    }

    void BloomRenderPass::OnResize(uint32_t width, uint32_t height) {
        m_Framebuffer->Resize(width, height);
        Invalidate(width, height);
    }

    void BloomRenderPass::Invalidate(uint32_t width, uint32_t height) {
        Ref<TextureData> data = Ref<TextureData>::Create();
        data->m_width = width;
        data->m_height = height;
        data->m_depth = 1;
        data->m_type = ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D;
        data->m_format = ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R16G16B16_HDR;
        m_BrightAreaImage = Texture2D::Create(data);
        m_BlurImage[0] = Texture2D::Create(data);
        m_BlurImage[1] = Texture2D::Create(data);
        m_BloomImage = Texture2D::Create(data);
    }
}  // namespace Ethereal