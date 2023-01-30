#include <Ethereal/src/Base/GlobalContext.h>
#include "BloomRenderPass.h"
#include "Core/Renderer/RenderResource.h"

namespace Ethereal
{
    void BloomRenderPass::Init(uint32_t width, uint32_t height) {
        m_Quad = RenderResource::Quad;

        TextureSpecification spec, depthSpec;
        spec.Format = ETHEREAL_PIXEL_FORMAT::PLACEHOLDER;
        depthSpec.Format = ETHEREAL_PIXEL_FORMAT::DEPTH;

        FramebufferSpecification fbSpec;
        // Input Image, BrightArea Image (Blur)
        fbSpec.ColorAttachments.PushAttachmentSpec(spec);
        fbSpec.DepthAttachment.SetAttachmentSpec(depthSpec);
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
        m_Framebuffer->Resize(m_Width, m_Height);
        m_Framebuffer->Bind();

        // Draw BrightArea Image
        m_Shader_Bright->Bind();
        m_Shader_Bright->SetInt("u_MainImage", 0);
        m_Shader_Bright->SetFloat("u_Threshold", 0.0f);  // 0.0 for blooming entire image
        m_MainImage->Bind(0);
        m_BrightAreaImage->BindToFramebuffer(0);
        RenderCommand::Clear();
        RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());

        // Blur Image

        m_Framebuffer->Resize(m_Width, m_Height);
        m_Framebuffer->Bind();
        // Down Sampler
        m_Shader_Blur->Bind();
        m_Shader_Blur->SetFloat("u_Threshold", m_Threshold);
        m_Shader_Blur->SetFloat("u_Knee", m_Knee);

        m_Shader_Blur->SetInt("u_DownSample", true);
        m_Shader_Blur->SetInt("o_image", 0);
        m_Shader_Blur->SetInt("i_image", 1);
        m_Shader_Blur->SetInt("i_DownSamplerImage", 2);

        m_DownSampledImage->BindImage(0, 0);
        m_BrightAreaImage->BindImage(1);
        m_Shader_Blur->SetInt("u_MipLevel", 0);

        RenderCommand::Clear();
        RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());

        for (int i = 1; i < m_MipLevels; i++) {
            auto mipWidth = static_cast<unsigned int>(m_Width * std::pow(0.5, i));
            auto mipHeight = static_cast<unsigned int>(m_Height * std::pow(0.5, i));

            m_Framebuffer->Resize(mipWidth, mipHeight);
            m_Framebuffer->Bind();

            m_DownSampledImage->BindImage(1, i - 1);
            m_Shader_Blur->SetInt("u_MipLevel", i);
            m_DownSampledImage->BindImage(0, i);

            RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());
        }

        // Up Sampler
        auto mipWidth = static_cast<unsigned int>(m_Width * std::pow(0.5, m_MipLevels - 2));
        auto mipHeight = static_cast<unsigned int>(m_Height * std::pow(0.5, m_MipLevels - 2));
        m_Framebuffer->Resize(mipWidth, mipHeight);
        m_Framebuffer->Bind();

        m_Shader_Blur->SetInt("u_DownSample", false);
        m_Shader_Blur->SetInt("u_MipLevel", m_MipLevels - 2);
        m_UpSampledImage->BindImage(0, m_MipLevels - 2);
        m_DownSampledImage->BindImage(1, m_MipLevels - 1);
        m_DownSampledImage->BindImage(2, m_MipLevels - 2);

        RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());

        for (int i = m_MipLevels - 3; i >= 0; --i) {
            mipWidth = static_cast<unsigned int>(m_Width * std::pow(0.5, i));
            mipHeight = static_cast<unsigned int>(m_Height * std::pow(0.5, i));

            m_Framebuffer->Resize(mipWidth, mipHeight);
            m_Framebuffer->Bind();

            m_Shader_Blur->SetInt("u_DownSample", false);
            m_Shader_Blur->SetInt("u_MipLevel", i);
            m_UpSampledImage->BindImage(0, i);
            m_UpSampledImage->BindImage(1, i + 1);
            m_DownSampledImage->BindImage(2, i);

            RenderCommand::DrawIndexed(m_Quad->GetMeshSource()->GetVertexArray(), m_Quad->GetMeshSource()->GetIndexBuffer()->GetCount());
        }

        // Merge Image
        m_Framebuffer->Resize(m_Width, m_Height);
        m_Framebuffer->Bind();

        m_Shader_Merge->Bind();
        m_Shader_Merge->SetFloat("u_Intensity", m_Intensity);

        m_Shader_Merge->SetInt("u_MainImage", 0);
        m_MainImage->Bind(0);
        m_Shader_Merge->SetInt("u_BlurImage", 1);
        m_UpSampledImage->Bind(1);
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
        m_Height = height;
        m_Width = width;
        Ref<TextureData> data = Ref<TextureData>::Create();
        data->m_width = width;
        data->m_height = height;
        data->m_depth = 1;
        data->m_type = ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D;
        data->m_format = ETHEREAL_PIXEL_FORMAT::R16G16B16A16_HDR;
        m_BrightAreaImage = Texture2D::Create(data);
        m_BloomImage = Texture2D::Create(data);

        m_UpSampledImage = Texture2D::Create(data);
        m_UpSampledImage->GenerateMipmaps();

        m_DownSampledImage = Texture2D::Create(data);
        m_DownSampledImage->GenerateMipmaps();

        m_MipLevels = log(std::min(height, width)) / log(2);
    }
}  // namespace Ethereal