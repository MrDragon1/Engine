#pragma once
#include "Engine/Renderer/Framebuffer.hpp"

namespace Engine
{
    class OpenGLFramebuffer : public Framebuffer {
      public:
        OpenGLFramebuffer(const FramebufferSpecification& spec);
        ~OpenGLFramebuffer();

        void Bind() override;
        void Unbind() override;

        const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
        uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

        void Invalidate();
      private:
        uint32_t m_RendererID;
        uint32_t m_ColorAttachment, m_DepthAttachment;
        FramebufferSpecification m_Specification;
    };
}  // namespace Engine