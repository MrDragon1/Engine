#pragma once
//#include "pch.h"
#include "Texture.h"
namespace Ethereal
{
    struct FramebufferTextureSpecification {
        FramebufferTextureSpecification() = default;
        FramebufferTextureSpecification(ETHEREAL_PIXEL_FORMAT format) : TextureFormat(format) {}
        ETHEREAL_PIXEL_FORMAT TextureFormat = ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32G32B32A32_FLOAT;
        // TODO: filtering/wrap
    };

    struct FramebufferAttachmentSpecification {
        FramebufferAttachmentSpecification() = default;
        FramebufferAttachmentSpecification(std::initializer_list<FramebufferTextureSpecification> attachments) : Attachments(attachments) {}

        std::vector<FramebufferTextureSpecification> Attachments;
    };

    struct FramebufferSpecification {
        uint32_t Width, Height;
        FramebufferAttachmentSpecification Attachments;
        uint32_t Samples = 1;
    };

    class Framebuffer : public RefCounted {
      public:
        virtual ~Framebuffer() = default;
        virtual void Bind() = 0;
        virtual void Unbind() = 0;
        virtual void Resize(uint32_t width, uint32_t height) = 0;

        virtual const FramebufferSpecification& GetSpecification() const = 0;
        virtual Ref<Texture> GetColorAttachment(uint32_t index = 0) const = 0;
        virtual Ref<Texture> GetDepthAttachment() const = 0;
        virtual void ClearAttachment(uint32_t attachmentIndex, int value) = 0;
        
        virtual int ReadPixel(uint32_t attachmentIndex, int x, int y) = 0;

        static Ref<Framebuffer> Create(const FramebufferSpecification& spec);
    };
}  // namespace Ethereal