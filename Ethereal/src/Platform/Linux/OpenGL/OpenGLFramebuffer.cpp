#include "pch.hpp"
#include "OpenGLFramebuffer.hpp"

#include <glad/glad.h>
#include "Ethereal/Core/Log.hpp"

namespace Ethereal
{
    static const uint32_t s_MaxFramebufferSize = 8192;
    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) : m_Specification(spec) {
        for (auto spec : m_Specification.Attachments.Attachments) {
            // TODO: Support more depth format
            if (spec.TextureFormat != ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH)
                m_ColorAttachmentSpecifications.emplace_back(spec);
            else
                m_DepthAttachmentSpecification = spec;
        }
        Invalidate();
    }

    OpenGLFramebuffer::~OpenGLFramebuffer() { glDeleteFramebuffers(1, &m_RendererID); }

    void OpenGLFramebuffer::Bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);
        glViewport(0, 0, m_Specification.Width, m_Specification.Height);
    }

    void OpenGLFramebuffer::Unbind() { glBindFramebuffer(GL_FRAMEBUFFER, 0); }

    void OpenGLFramebuffer::Invalidate() {
        if (m_RendererID) {
            glDeleteFramebuffers(1, &m_RendererID);
            for (auto& colorAttachment : m_ColorAttachments) {
                colorAttachment.Reset();
            }
            m_DepthAttachment.Reset();

            m_ColorAttachments.clear();
            m_DepthAttachment = nullptr;
        }
        glCreateFramebuffers(1, &m_RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererID);

        // Attachments
        if (m_ColorAttachmentSpecifications.size()) {
            m_ColorAttachments.resize(m_ColorAttachmentSpecifications.size());
            for (size_t i = 0; i < m_ColorAttachments.size(); i++) {
                Ref<TextureData> data = Ref<TextureData>::Create();
                data->m_width = m_Specification.Width;
                data->m_height = m_Specification.Height;
                data->m_depth = 1;
                data->m_type = ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D;
                data->m_format = m_ColorAttachmentSpecifications[i].TextureFormat;
                if (m_ColorAttachmentSpecifications[i].TextureFormat == ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_PLACEHOLDER) continue;
                m_ColorAttachments[i] = Texture2D::Create(data);
                m_ColorAttachments[i]->Bind();
                glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorAttachments[i]->GetRendererID(), 0);
            }
        }

        if (m_DepthAttachmentSpecification.TextureFormat == ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH) {
            Ref<TextureData> data = Ref<TextureData>::Create();
            data->m_width = m_Specification.Width;
            data->m_height = m_Specification.Height;
            data->m_depth = 1;
            data->m_type = ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D;
            data->m_format = m_DepthAttachmentSpecification.TextureFormat;
            m_DepthAttachment = Texture2D::Create(data);
            m_DepthAttachment->Bind();
            glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_DepthAttachment->GetRendererID(), 0);
        }

        if (m_ColorAttachments.size() > 1) {
            ET_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Only support most 4 color attachments!");
            GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        } else if (m_ColorAttachments.empty()) {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }

        ET_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFramebuffer::Resize(uint32_t width, uint32_t height) {
        if (width == 0 || height == 0 || width > s_MaxFramebufferSize || height > s_MaxFramebufferSize) {
            ET_CORE_WARN("Attempted to resize framebuffer to {0} {1}!", width, height);
            return;
        }
        m_Specification.Width = width;
        m_Specification.Height = height;

        Invalidate();
    }

    int OpenGLFramebuffer::ReadPixel(uint32_t attachmentIndex, int x, int y) {
        Bind();
        ET_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixelData;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
        Unbind();
        return pixelData;
    }

    void OpenGLFramebuffer::ClearAttachment(uint32_t attachmentIndex, int value) {
        ET_CORE_ASSERT(attachmentIndex < m_ColorAttachments.size());
        m_ColorAttachments[attachmentIndex]->Clear(value);
    }
}  // namespace Ethereal