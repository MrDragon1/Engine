#include "pch.h"
#include "OpenGLFramebuffer.h"

#include <glad/glad.h>
#include "Ethereal/src/Base/Log.h"

namespace Ethereal
{
    static const uint32_t s_MaxFramebufferSize = 8192;
    OpenGLFramebuffer::OpenGLFramebuffer(const FramebufferSpecification& spec) : m_Specification(spec) {
        for (auto spec : m_Specification.ColorAttachments.Attachments) {
            // TODO: Support more depth format
            m_ColorAttachmentSpecifications.emplace_back(spec);
        }
        m_DepthAttachmentSpecification = m_Specification.DepthAttachment.Attachments[0];
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
                // TODO: Deprecated TextureData
                Ref<TextureData> data = Ref<TextureData>::Create();
                data->m_width = m_ColorAttachmentSpecifications[i].Width ? m_ColorAttachmentSpecifications[i].Width : m_Specification.Width;
                data->m_height = m_ColorAttachmentSpecifications[i].Height ? m_ColorAttachmentSpecifications[i].Height : m_Specification.Height;
                data->m_depth = m_ColorAttachmentSpecifications[i].Depth;
                data->m_type = m_ColorAttachmentSpecifications[i].Type;
                data->m_format = m_ColorAttachmentSpecifications[i].Format;
                if (m_ColorAttachmentSpecifications[i].Format == ETHEREAL_PIXEL_FORMAT::PLACEHOLDER) continue;
                if (data->m_type == ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D) {
                    m_ColorAttachments[i] = Texture2D::Create(data);
                    m_ColorAttachments[i]->Bind();
                    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_ColorAttachments[i]->GetRendererID(), 0);
                } else if (data->m_type == ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D_ARRAY) {
                    m_ColorAttachments[i] = Texture3D::Create(data);
                    m_ColorAttachments[i]->Bind();
                    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, m_ColorAttachments[i]->GetRendererID(), 0);
                }
            }
        }

        if (m_DepthAttachmentSpecification.Format == ETHEREAL_PIXEL_FORMAT::DEPTH) {
            Ref<TextureData> data = Ref<TextureData>::Create();
            data->m_width = m_DepthAttachmentSpecification.Width ? m_DepthAttachmentSpecification.Width : m_Specification.Width;
            data->m_height = m_DepthAttachmentSpecification.Height ? m_DepthAttachmentSpecification.Height : m_Specification.Height;
            data->m_depth = m_DepthAttachmentSpecification.Depth;
            data->m_type = m_DepthAttachmentSpecification.Type;
            data->m_format = m_DepthAttachmentSpecification.Format;

            if (data->m_type == ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D) {
                m_DepthAttachment = Texture2D::Create(data);
                m_DepthAttachment->Bind();
                glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetRendererID(), 0);
            } else if (data->m_type == ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D_ARRAY) {
                m_DepthAttachment = Texture3D::Create(data);
                m_DepthAttachment->Bind();
                glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthAttachment->GetRendererID(), 0);
            }
        }

        if (m_ColorAttachments.size() > 1) {
            ET_CORE_ASSERT(m_ColorAttachments.size() <= 4, "Only support most 4 color attachments!");
            GLenum buffers[4] = {GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3};
            glDrawBuffers(m_ColorAttachments.size(), buffers);
        } else if (m_ColorAttachments.empty()) {
            // Only depth-pass
            glDrawBuffer(GL_NONE);
        }
        auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        ET_CORE_ASSERT(fboStatus == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");

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