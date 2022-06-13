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

        virtual void Resize(uint32_t width, uint32_t height) override;

        const FramebufferSpecification& GetSpecification() const override { return m_Specification; }
        uint32_t GetColorAttachmentRendererID() const override { return m_ColorAttachment; }

        void Invalidate();

      private:
        uint32_t m_RendererID = 0;
        uint32_t m_ColorAttachment = 0, m_DepthAttachment = 0;
        FramebufferSpecification m_Specification;
    };
}  // namespace Engine