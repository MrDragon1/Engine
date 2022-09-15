#pragma once
// #include "pch.h"
#include "Texture.h"
namespace Ethereal
{
    struct TextureSpecification {
        TextureSpecification() = default;
        uint32_t Width{0};
        uint32_t Height{0};
        uint32_t Depth{1};
        uint32_t MipLevels{0};
        void* Data{nullptr};

        ETHEREAL_PIXEL_FORMAT Format{ETHEREAL_PIXEL_FORMAT::PLACEHOLDER};
        ETHEREAL_IMAGE_TYPE Type{ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D};

        ETHEREAL_WARP_FORMAT WarpFormat{ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_CLAMP_TO_EDGE};
        ETHEREAL_FILTER_FORMAT FilterFormat{ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR};
    };

    struct FramebufferAttachmentSpecification {
        FramebufferAttachmentSpecification() = default;
        void PushAttachmentSpec(TextureSpecification spec) { Attachments.push_back(spec); }
        void SetAttachmentSpec(TextureSpecification spec) {
            Attachments.clear();
            Attachments.push_back(spec);
        }
        std::vector<TextureSpecification> Attachments;
    };

    struct FramebufferSpecification {
        uint32_t Width, Height;
        FramebufferAttachmentSpecification ColorAttachments;
        FramebufferAttachmentSpecification DepthAttachment;
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