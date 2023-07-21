#pragma once
#include "Backend/DriverBase.h"
#include "Backend/Vulkan/VulkanUtils.h"
#include "Backend/Vulkan/VulkanContext.h"
#include "Backend/BufferDescriptor.h"
namespace Ethereal {
namespace Backend {

class VulkanTexture : public Texture {
   public:
    using Texture::Texture;
    VulkanTexture(Ref<VulkanContext> context, uint32_t width, uint32_t height, uint32_t depth,
                  uint32_t levels, TextureFormat format, TextureUsage usage, TextureType type);
    ~VulkanTexture();
    VkImageView GetImageView(VkImageSubresourceRange range, VkImageViewType viewType,
                             VkComponentMapping swizzle);

    void UpdateData(const PixelBufferDescriptor& data, uint32_t width, uint32_t height,
                    uint32_t depth, uint32_t xoffset, uint32_t yoffset, uint32_t zoffset,
                    uint32_t miplevel);

    VulkanLayout GetLayout(uint32_t layer, uint32_t level);

    void TransitionLayout(VkCommandBuffer buffer, VulkanLayout newLayout,
                          VkImageSubresourceRange range);

    VkImageView GetPrimaryImageView() const { return mCachedImageViews.at(mPrimaryViewRange); }
    VulkanLayout GetPrimaryImageLayout() {
        return GetLayout(mPrimaryViewRange.baseArrayLayer, mPrimaryViewRange.baseMipLevel);
    }
    VkImageAspectFlags GetImageAspect() const;
    VkImage GetVkImage() { return mImage; }
    VkImageSubresourceRange GetPrimaryRange() { return mPrimaryViewRange; }
    VkImageView GetAttachmentView(VkImageSubresourceRange);
    VkFormat GetVkFormat() { return mVkFormat; }

   private:
    VkImage mImage;
    VmaAllocation mAllocation;

    VkImageSubresourceRange mPrimaryViewRange;
    std::map<VkImageSubresourceRange, VkImageView> mCachedImageViews;
    std::map<uint32_t, VulkanLayout> mSubresourceLayouts;
    VkFormat mVkFormat;
    Ref<VulkanContext> mContext;
};
}  // namespace Backend
}  // namespace Ethereal