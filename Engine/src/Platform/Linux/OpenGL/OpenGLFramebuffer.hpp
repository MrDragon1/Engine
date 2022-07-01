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
        uint32_t GetColorAttachmentRendererID(uint32_t index = 0) const override {
            ENGINE_CORE_ASSERT(index < m_ColorAttachments.size());
            return m_ColorAttachments[index];
        }

        void Invalidate();

      private:
        uint32_t m_RendererID = 0;
        FramebufferSpecification m_Specification;

        std::vector<FramebufferTextureSpecification> m_ColorAttachmentSpecifications;
        FramebufferTextureSpecification m_DepthAttachmentSpecification = FramebufferTextureFormat::None;

        std::vector<uint32_t> m_ColorAttachments;
        uint32_t m_DepthAttachment = 0;
    };
}  // namespace Engine