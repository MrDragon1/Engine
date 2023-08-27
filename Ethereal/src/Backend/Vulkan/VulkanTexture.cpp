#include "VulkanTexture.h"
#include "Backend/Vulkan/VulkanAllocator.h"

namespace Ethereal {
namespace Backend {

VulkanTexture::VulkanTexture(Ref<VulkanContext> context, uint32_t width, uint32_t height,
                             uint32_t depth, uint32_t levels, TextureFormat format,
                             TextureUsage usage, TextureType type)
    : Texture(width, height, depth, levels, format, usage, type), mContext(context) {
    mVkFormat = VulkanUtils::ResolveTextureFormat(format);
    VkImageCreateInfo imageInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .imageType = type == TextureType::TEXTURE_3D ? VK_IMAGE_TYPE_3D : VK_IMAGE_TYPE_2D,
        .format = mVkFormat,
        .extent = {width, height, depth},
        .mipLevels = levels,
        .arrayLayers = 1,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = 0};
    if (type == TextureType::TEXTURE_CUBEMAP) {
        imageInfo.arrayLayers = 6;
        imageInfo.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
        imageInfo.extent = {width, height, 1};
    }
    if (type == TextureType::TEXTURE_2D_ARRAY) {
        imageInfo.arrayLayers = depth;
        imageInfo.extent.depth = 1;
    }

    const VkImageUsageFlags blittable =
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    if (any(usage & TextureUsage::SAMPLEABLE)) {
        imageInfo.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    }
    if (any(usage & TextureUsage::COLOR_ATTACHMENT)) {
        imageInfo.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | blittable;
        if (any(usage & TextureUsage::SUBPASS_INPUT)) {
            imageInfo.usage |= VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT;
        }
    }
    if (any(usage & TextureUsage::STENCIL_ATTACHMENT)) {
        imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }
    if (any(usage & TextureUsage::UPLOADABLE)) {
        imageInfo.usage |= blittable;
    }
    if (any(usage & TextureUsage::DEPTH_ATTACHMENT)) {
        imageInfo.usage |= blittable;
        imageInfo.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    }

    // TODO: MSAA need to set sample count
    imageInfo.samples = (VkSampleCountFlagBits)1;

    mAllocation = VulkanAllocator::AllocateImage(imageInfo, VMA_MEMORY_USAGE_GPU_ONLY, mImage);

    // Spec out the "primary" VkImageView that shaders use to sample from the image.
    mPrimaryViewRange.aspectMask = GetImageAspect();
    mPrimaryViewRange.baseMipLevel = 0;
    mPrimaryViewRange.levelCount = levels;
    mPrimaryViewRange.baseArrayLayer = 0;
    if (type == TextureType::TEXTURE_CUBEMAP) {
        mPrimaryViewRange.layerCount = 6;
    } else if (type == TextureType::TEXTURE_2D_ARRAY) {
        mPrimaryViewRange.layerCount = depth;
    } else if (type == TextureType::TEXTURE_3D) {
        mPrimaryViewRange.layerCount = 1;
    } else {
        mPrimaryViewRange.layerCount = 1;
    }

    GetImageView(mPrimaryViewRange, VulkanUtils::GetViewType(type), {});

    if (any(usage & (TextureUsage::COLOR_ATTACHMENT | TextureUsage::DEPTH_ATTACHMENT))) {
        const uint32_t layers = mPrimaryViewRange.layerCount;
        VkImageSubresourceRange range = {GetImageAspect(), 0, levels, 0, layers};
        VkCommandBuffer cmdbuf = mContext->mDevice->GetCommandBuffer(true);
        TransitionLayout(cmdbuf, VulkanUtils::GetDefaultLayout(usage), range);
        mContext->mDevice->FlushCommandBuffer(cmdbuf);
    }
}

VulkanTexture::~VulkanTexture() {
    VulkanAllocator::DestroyImage(mImage, mAllocation);
    for (auto entry : mCachedImageViews) {
        vkDestroyImageView(mContext->mDevice->GetDevice(), entry.second, nullptr);
    }
}

VkImageView VulkanTexture::GetImageView(VkImageSubresourceRange range, VkImageViewType viewType,
                                        VkComponentMapping swizzle) {
    auto iter = mCachedImageViews.find(range);
    if (iter != mCachedImageViews.end()) {
        return iter->second;
    }
    VkImageViewCreateInfo viewInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = mImage,
        .viewType = viewType,
        .format = mVkFormat,
        .components = swizzle,
        .subresourceRange = range,
    };
    VkImageView imageView;
    vkCreateImageView(mContext->mDevice->GetDevice(), &viewInfo, nullptr, &imageView);
    mCachedImageViews.emplace(range, imageView);
    return imageView;
}

void VulkanTexture::UpdateData(const PixelBufferDescriptor& data, uint32_t width, uint32_t height,
                               uint32_t depth, uint32_t xoffset, uint32_t yoffset, uint32_t zoffset,
                               uint32_t miplevel) {
    ET_CORE_ASSERT(width <= this->width && height <= this->height);
    ET_CORE_ASSERT(depth <= this->depth * ((type == TextureType::TEXTURE_CUBEMAP) ? 6 : 1));

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    VkBufferCreateInfo bufferCreateInfo = {};
    bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferCreateInfo.size = data.size;
    bufferCreateInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    auto allocation = VulkanAllocator::AllocateBuffer(bufferCreateInfo, VMA_MEMORY_USAGE_CPU_TO_GPU,
                                                      stagingBuffer);

    uint8_t* mapdata = VulkanAllocator::MapMemory<uint8_t>(allocation);
    memcpy(mapdata, data.buffer, data.size);
    VulkanAllocator::UnmapMemory(allocation);

    VkBufferImageCopy copyRegion = {
        .bufferOffset = {},
        .bufferRowLength = {},
        .bufferImageHeight = {},
        .imageSubresource = {.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                             .mipLevel = miplevel,
                             .baseArrayLayer = 0,
                             .layerCount = 1},
        .imageOffset = {int32_t(xoffset), int32_t(yoffset), int32_t(zoffset)},
        .imageExtent = {width, height, depth}};

    VkImageSubresourceRange transitionRange = {.aspectMask = GetImageAspect(),
                                               .baseMipLevel = miplevel,
                                               .levelCount = 1,
                                               .baseArrayLayer = 0,
                                               .layerCount = 1};

    // Vulkan specifies subregions for 3D textures differently than from 2D arrays.
    if (type == TextureType::TEXTURE_2D_ARRAY || type == TextureType::TEXTURE_CUBEMAP) {
        copyRegion.imageOffset.z = 0;
        copyRegion.imageExtent.depth = 1;
        copyRegion.imageSubresource.baseArrayLayer = zoffset;
        copyRegion.imageSubresource.layerCount = depth;
        transitionRange.baseArrayLayer = zoffset;
        transitionRange.layerCount = depth;
    }

    VulkanLayout const newLayout = VulkanLayout::TRANSFER_DST;
    VulkanLayout nextLayout = GetLayout(0, miplevel);
    VkImageLayout const newVkLayout = VulkanUtils::GetVkImageLayout(newLayout);

    if (nextLayout == VulkanLayout::UNDEFINED) {
        nextLayout = VulkanLayout::READ_WRITE;
    }

    auto cmdbuf = mContext->mDevice->GetCommandBuffer(true);
    TransitionLayout(cmdbuf, newLayout, transitionRange);

    vkCmdCopyBufferToImage(cmdbuf, stagingBuffer, mImage, newVkLayout, 1, &copyRegion);
    TransitionLayout(cmdbuf, nextLayout, transitionRange);

    mContext->mDevice->FlushCommandBuffer(cmdbuf);

    // Clean up staging resources
    vkDestroyBuffer(mContext->mDevice->GetDevice(), stagingBuffer, nullptr);
}

VulkanLayout VulkanTexture::GetLayout(uint32_t layer, uint32_t level) {
    ET_CORE_ASSERT(level <= 0xffff && layer <= 0xffff);
    const uint32_t key = (layer << 16) | level;
    if (mSubresourceLayouts.find(key) == mSubresourceLayouts.end()) {
        return VulkanLayout::UNDEFINED;
    }
    return mSubresourceLayouts[key];
}

void VulkanTexture::TransitionLayout(VkCommandBuffer cmdbuffer, VulkanLayout newLayout,
                                     VkImageSubresourceRange range) {
    VulkanLayout oldLayout = GetLayout(range.baseArrayLayer, range.baseMipLevel);

    VulkanUtils::TransitionLayout(cmdbuffer, {
                                                 .image = mImage,
                                                 .oldLayout = oldLayout,
                                                 .newLayout = newLayout,
                                                 .subresources = range,
                                             });

    uint32_t const firstLayer = range.baseArrayLayer;
    uint32_t const lastLayer = firstLayer + range.layerCount;
    uint32_t const firstLevel = range.baseMipLevel;
    uint32_t const lastLevel = firstLevel + range.levelCount;

    ET_CORE_ASSERT(firstLevel <= 0xffff && lastLevel <= 0xffff);
    ET_CORE_ASSERT(firstLayer <= 0xffff && lastLayer <= 0xffff);

    if (newLayout == VulkanLayout::UNDEFINED) {
        for (uint32_t layer = firstLayer; layer < lastLayer; ++layer) {
            uint32_t const first = (layer << 16) | firstLevel;
            uint32_t const last = (layer << 16) | lastLevel;
            for (uint32_t i = first; i < last; ++i) {
                mSubresourceLayouts.erase(i);
            }
        }
    } else {
        for (uint32_t layer = firstLayer; layer < lastLayer; ++layer) {
            uint32_t const first = (layer << 16) | firstLevel;
            uint32_t const last = (layer << 16) | lastLevel;
            for (uint32_t i = first; i < last; i++) {
                mSubresourceLayouts[i] = newLayout;
            }
        }
    }
}

VkImageAspectFlags VulkanTexture::GetImageAspect() const {
    return VulkanUtils::GetImageAspect(mVkFormat);
}

VkImageView VulkanTexture::GetAttachmentView(VkImageSubresourceRange range) {
    range.levelCount = 1;
    // range.layerCount = 1;
    if (range.layerCount == 1) {
        return GetImageView(range, VK_IMAGE_VIEW_TYPE_2D, {});
    } else
        return GetImageView(range, VulkanUtils::GetViewType(type), {});
}

}  // namespace Backend
}  // namespace Ethereal
