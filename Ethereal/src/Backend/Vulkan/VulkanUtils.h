#pragma once
#include "vulkan/vulkan.h"
#include "vma/vk_mem_alloc.h"
#include "Backend/DriverBase.h"
namespace Ethereal {

namespace Backend {
const int MAX_FRAMES_IN_FLIGHT = 2;

enum class VulkanLayout : uint8_t {
    // The initial layout after the creation of the VkImage. We use this to denote the state before
    // any transition.
    UNDEFINED,
    // Fragment/vertex shader accessible layout for reading and writing.
    READ_WRITE,
    // Fragment/vertex shader accessible layout for reading only.
    READ_ONLY,
    // For the source of a copy operation.
    TRANSFER_SRC,
    // For the destination of a copy operation.
    TRANSFER_DST,
    // For using a depth texture as an attachment.
    DEPTH_ATTACHMENT,
    // For using a depth texture both as an attachment and as a sampler.
    DEPTH_SAMPLER,
    // For swapchain images that will be presented.
    PRESENT,
    // For color attachments, but also used when the image is a sampler.
    // TODO: explore separate layout policies for attachment+sampling and just attachment.
    COLOR_ATTACHMENT,
    // For color attachment MSAA resolves.
    COLOR_ATTACHMENT_RESOLVE,
};

struct VulkanLayoutTransition {
    VkImage image;
    VulkanLayout oldLayout;
    VulkanLayout newLayout;
    VkImageSubresourceRange subresources;
};

namespace VulkanUtils {

inline VkImageLayout GetVkImageLayout(VulkanLayout layout) {
    switch (layout) {
        case VulkanLayout::UNDEFINED:
            return VK_IMAGE_LAYOUT_UNDEFINED;
        case VulkanLayout::READ_WRITE:
            return VK_IMAGE_LAYOUT_GENERAL;
        case VulkanLayout::READ_ONLY:
            return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        case VulkanLayout::TRANSFER_SRC:
            return VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        case VulkanLayout::TRANSFER_DST:
            return VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        case VulkanLayout::DEPTH_ATTACHMENT:
            return VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        case VulkanLayout::DEPTH_SAMPLER:
            return VK_IMAGE_LAYOUT_GENERAL;
        case VulkanLayout::PRESENT:
            return VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        // Filament sometimes samples from one miplevel while writing to another level in the same
        // texture (e.g. bloom does this). Moreover we'd like to avoid lots of expensive layout
        // transitions. So, keep it simple and use GENERAL for all color-attachable textures.
        case VulkanLayout::COLOR_ATTACHMENT:
            return VK_IMAGE_LAYOUT_GENERAL;
        case VulkanLayout::COLOR_ATTACHMENT_RESOLVE:
            return VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    }
}

inline std::tuple<VkAccessFlags, VkAccessFlags, VkPipelineStageFlags, VkPipelineStageFlags,
                  VkImageLayout, VkImageLayout>
GetVkTransition(const VulkanLayoutTransition& transition) {
    VkAccessFlags srcAccessMask, dstAccessMask;
    VkPipelineStageFlags srcStage, dstStage;

    switch (transition.oldLayout) {
        case VulkanLayout::UNDEFINED:
            srcAccessMask = 0;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VulkanLayout::COLOR_ATTACHMENT:
            srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        case VulkanLayout::READ_WRITE:
            srcAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VulkanLayout::READ_ONLY:
            srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VulkanLayout::TRANSFER_SRC:
            srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VulkanLayout::TRANSFER_DST:
            srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VulkanLayout::DEPTH_ATTACHMENT:
            srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            srcStage = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
            break;
        case VulkanLayout::DEPTH_SAMPLER:
            srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VulkanLayout::PRESENT:
        case VulkanLayout::COLOR_ATTACHMENT_RESOLVE:
            srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            srcStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
    }

    switch (transition.newLayout) {
        case VulkanLayout::COLOR_ATTACHMENT:
            dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                       VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
            break;
        case VulkanLayout::READ_WRITE:
            dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VulkanLayout::READ_ONLY:
            dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
            dstStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
            break;
        case VulkanLayout::TRANSFER_SRC:
            dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VulkanLayout::TRANSFER_DST:
            dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            break;
        case VulkanLayout::DEPTH_ATTACHMENT:
            dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dstStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            break;
        case VulkanLayout::DEPTH_SAMPLER:
            dstAccessMask =
                VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
            dstStage =
                VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
            break;
        case VulkanLayout::PRESENT:
        case VulkanLayout::COLOR_ATTACHMENT_RESOLVE:
        case VulkanLayout::UNDEFINED:
            dstAccessMask = 0;
            dstStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            break;
    }

    return std::make_tuple(srcAccessMask, dstAccessMask, srcStage, dstStage,
                           GetVkImageLayout(transition.oldLayout),
                           GetVkImageLayout(transition.newLayout));
}

static void TransitionLayout(VkCommandBuffer cmdbuffer, VulkanLayoutTransition transition) {
    if (transition.oldLayout == transition.newLayout) {
        return;
    }
    auto [srcAccessMask, dstAccessMask, srcStage, dstStage, oldLayout, newLayout] =
        GetVkTransition(transition);

    ET_CORE_ASSERT(transition.image != VK_NULL_HANDLE, "No image for transition");
    VkImageMemoryBarrier barrier = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .srcAccessMask = srcAccessMask,
        .dstAccessMask = dstAccessMask,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = transition.image,
        .subresourceRange = transition.subresources,
    };
    vkCmdPipelineBarrier(cmdbuffer, srcStage, dstStage, 0, 0, nullptr, 0, nullptr, 1, &barrier);
}

static VkFormat ResolveTextureFormat(const TextureFormat& format) {
    switch (format) {
        case TextureFormat::R8:
            return VK_FORMAT_R8_UNORM;
        case TextureFormat::R8G8B8_UNORM:
            return VK_FORMAT_R8G8B8_UNORM;
        case TextureFormat::R8G8B8A8_UNORM:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case TextureFormat::R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case TextureFormat::R32G32_FLOAT:
            return VK_FORMAT_R32G32_SFLOAT;
        case TextureFormat::R32G32B32_FLOAT:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case TextureFormat::R32G32B32A32_FLOAT:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case TextureFormat::R32_INTEGER:
            return VK_FORMAT_R32_UINT;
        case TextureFormat::DEPTH:
            return VK_FORMAT_D32_SFLOAT;
        case TextureFormat::R16:
            return VK_FORMAT_R16_UNORM;
        case TextureFormat::R16G16B16A16_HDR:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case TextureFormat::R16G16B16_HDR:
            return VK_FORMAT_R16G16B16_SFLOAT;
    }
}

static VkImageAspectFlags GetImageAspect(VkFormat format) {
    switch (format) {
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
            return VK_IMAGE_ASPECT_DEPTH_BIT;
        case VK_FORMAT_S8_UINT:
            return VK_IMAGE_ASPECT_STENCIL_BIT;
        default:
            return VK_IMAGE_ASPECT_COLOR_BIT;
    }
}

static VkImageViewType GetViewType(TextureType target) {
    switch (target) {
        case TextureType::TEXTURE_CUBEMAP:
            return VK_IMAGE_VIEW_TYPE_CUBE;
        case TextureType::TEXTURE_2D_ARRAY:
            return VK_IMAGE_VIEW_TYPE_2D_ARRAY;
        case TextureType::TEXTURE_3D:
            return VK_IMAGE_VIEW_TYPE_3D;
        default:
            return VK_IMAGE_VIEW_TYPE_2D;
    }
}

inline static VulkanLayout GetDefaultLayout(TextureUsage usage) {
    if (any(usage & TextureUsage::DEPTH_ATTACHMENT)) {
        return VulkanLayout::DEPTH_ATTACHMENT;
    }

    if (any(usage & TextureUsage::COLOR_ATTACHMENT)) {
        return VulkanLayout::COLOR_ATTACHMENT;
    }
    // Finally, the layout for an immutable texture is optimal read-only.
    return VulkanLayout::READ_ONLY;
}

static VkFormat GetVkFormatLinear(VkFormat format) {
    switch (format) {
        case VK_FORMAT_R8_SRGB:
            return VK_FORMAT_R8_UNORM;
        case VK_FORMAT_R8G8_SRGB:
            return VK_FORMAT_R8G8_UNORM;
        case VK_FORMAT_R8G8B8_SRGB:
            return VK_FORMAT_R8G8B8_UNORM;
        case VK_FORMAT_B8G8R8_SRGB:
            return VK_FORMAT_B8G8R8_UNORM;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return VK_FORMAT_B8G8R8A8_UNORM;
        case VK_FORMAT_A8B8G8R8_SRGB_PACK32:
            return VK_FORMAT_A8B8G8R8_UNORM_PACK32;
        case VK_FORMAT_BC1_RGB_SRGB_BLOCK:
            return VK_FORMAT_BC1_RGB_UNORM_BLOCK;
        case VK_FORMAT_BC1_RGBA_SRGB_BLOCK:
            return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
        case VK_FORMAT_BC2_SRGB_BLOCK:
            return VK_FORMAT_BC2_UNORM_BLOCK;
        case VK_FORMAT_BC3_SRGB_BLOCK:
            return VK_FORMAT_BC3_UNORM_BLOCK;
        case VK_FORMAT_BC7_SRGB_BLOCK:
            return VK_FORMAT_BC7_UNORM_BLOCK;
        case VK_FORMAT_ETC2_R8G8B8_SRGB_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8_UNORM_BLOCK;
        case VK_FORMAT_ETC2_R8G8B8A1_SRGB_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A1_UNORM_BLOCK;
        case VK_FORMAT_ETC2_R8G8B8A8_SRGB_BLOCK:
            return VK_FORMAT_ETC2_R8G8B8A8_UNORM_BLOCK;
        case VK_FORMAT_ASTC_4x4_SRGB_BLOCK:
            return VK_FORMAT_ASTC_4x4_UNORM_BLOCK;
        case VK_FORMAT_ASTC_5x4_SRGB_BLOCK:
            return VK_FORMAT_ASTC_5x4_UNORM_BLOCK;
        case VK_FORMAT_ASTC_5x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_5x5_UNORM_BLOCK;
        case VK_FORMAT_ASTC_6x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_6x5_UNORM_BLOCK;
        case VK_FORMAT_ASTC_6x6_SRGB_BLOCK:
            return VK_FORMAT_ASTC_6x6_UNORM_BLOCK;
        case VK_FORMAT_ASTC_8x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_8x5_UNORM_BLOCK;
        case VK_FORMAT_ASTC_8x6_SRGB_BLOCK:
            return VK_FORMAT_ASTC_8x6_UNORM_BLOCK;
        case VK_FORMAT_ASTC_8x8_SRGB_BLOCK:
            return VK_FORMAT_ASTC_8x8_UNORM_BLOCK;
        case VK_FORMAT_ASTC_10x5_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x5_UNORM_BLOCK;
        case VK_FORMAT_ASTC_10x6_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x6_UNORM_BLOCK;
        case VK_FORMAT_ASTC_10x8_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x8_UNORM_BLOCK;
        case VK_FORMAT_ASTC_10x10_SRGB_BLOCK:
            return VK_FORMAT_ASTC_10x10_UNORM_BLOCK;
        case VK_FORMAT_ASTC_12x10_SRGB_BLOCK:
            return VK_FORMAT_ASTC_12x10_UNORM_BLOCK;
        case VK_FORMAT_ASTC_12x12_SRGB_BLOCK:
            return VK_FORMAT_ASTC_12x12_UNORM_BLOCK;
        case VK_FORMAT_PVRTC1_2BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_2BPP_UNORM_BLOCK_IMG;
        case VK_FORMAT_PVRTC1_4BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC1_4BPP_UNORM_BLOCK_IMG;
        case VK_FORMAT_PVRTC2_2BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_2BPP_UNORM_BLOCK_IMG;
        case VK_FORMAT_PVRTC2_4BPP_SRGB_BLOCK_IMG:
            return VK_FORMAT_PVRTC2_4BPP_UNORM_BLOCK_IMG;
        default:
            return format;
    }
    return format;
}

static VkFormat GetVkFormat(ElementType type, bool normalized, bool integer) {
    if (normalized) {
        switch (type) {
            // Single Component Types
            case ElementType::BYTE:
                return VK_FORMAT_R8_SNORM;
            case ElementType::UBYTE:
                return VK_FORMAT_R8_UNORM;
            case ElementType::SHORT:
                return VK_FORMAT_R16_SNORM;
            case ElementType::USHORT:
                return VK_FORMAT_R16_UNORM;
            // Two Component Types
            case ElementType::BYTE2:
                return VK_FORMAT_R8G8_SNORM;
            case ElementType::UBYTE2:
                return VK_FORMAT_R8G8_UNORM;
            case ElementType::SHORT2:
                return VK_FORMAT_R16G16_SNORM;
            case ElementType::USHORT2:
                return VK_FORMAT_R16G16_UNORM;
            // Three Component Types
            case ElementType::BYTE3:
                return VK_FORMAT_R8G8B8_SNORM;  // NOT MINSPEC
            case ElementType::UBYTE3:
                return VK_FORMAT_R8G8B8_UNORM;  // NOT MINSPEC
            case ElementType::SHORT3:
                return VK_FORMAT_R16G16B16_SNORM;  // NOT MINSPEC
            case ElementType::USHORT3:
                return VK_FORMAT_R16G16B16_UNORM;  // NOT MINSPEC
            // Four Component Types
            case ElementType::BYTE4:
                return VK_FORMAT_R8G8B8A8_SNORM;
            case ElementType::UBYTE4:
                return VK_FORMAT_R8G8B8A8_UNORM;
            case ElementType::SHORT4:
                return VK_FORMAT_R16G16B16A16_SNORM;
            case ElementType::USHORT4:
                return VK_FORMAT_R16G16B16A16_UNORM;
            default:
                ET_CORE_ASSERT(false, "Normalized format does not exist.");
                return VK_FORMAT_UNDEFINED;
        }
    }
    switch (type) {
        // Single Component Types
        case ElementType::BYTE:
            return integer ? VK_FORMAT_R8_SINT : VK_FORMAT_R8_SSCALED;
        case ElementType::UBYTE:
            return integer ? VK_FORMAT_R8_UINT : VK_FORMAT_R8_USCALED;
        case ElementType::SHORT:
            return integer ? VK_FORMAT_R16_SINT : VK_FORMAT_R16_SSCALED;
        case ElementType::USHORT:
            return integer ? VK_FORMAT_R16_UINT : VK_FORMAT_R16_USCALED;
        // case ElementType::HALF:
        //      return VK_FORMAT_R16_SFLOAT;
        case ElementType::INT:
            return VK_FORMAT_R32_SINT;
        case ElementType::UINT:
            return VK_FORMAT_R32_UINT;
        case ElementType::FLOAT:
            return VK_FORMAT_R32_SFLOAT;
        // Two Component Types
        case ElementType::BYTE2:
            return integer ? VK_FORMAT_R8G8_SINT : VK_FORMAT_R8G8_SSCALED;
        case ElementType::UBYTE2:
            return integer ? VK_FORMAT_R8G8_UINT : VK_FORMAT_R8G8_USCALED;
        case ElementType::SHORT2:
            return integer ? VK_FORMAT_R16G16_SINT : VK_FORMAT_R16G16_SSCALED;
        case ElementType::USHORT2:
            return integer ? VK_FORMAT_R16G16_UINT : VK_FORMAT_R16G16_USCALED;
        // case ElementType::HALF2:
        //     return VK_FORMAT_R16G16_SFLOAT;
        case ElementType::FLOAT2:
            return VK_FORMAT_R32G32_SFLOAT;
        // Three Component Types
        case ElementType::BYTE3:
            return VK_FORMAT_R8G8B8_SINT;  // NOT MINSPEC
        case ElementType::UBYTE3:
            return VK_FORMAT_R8G8B8_UINT;  // NOT MINSPEC
        case ElementType::SHORT3:
            return VK_FORMAT_R16G16B16_SINT;  // NOT MINSPEC
        case ElementType::USHORT3:
            return VK_FORMAT_R16G16B16_UINT;  // NOT MINSPEC
        // case ElementType::HALF3:
        //     return VK_FORMAT_R16G16B16_SFLOAT;  // NOT MINSPEC
        case ElementType::FLOAT3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        // Four Component Types
        case ElementType::BYTE4:
            return integer ? VK_FORMAT_R8G8B8A8_SINT : VK_FORMAT_R8G8B8A8_SSCALED;
        case ElementType::UBYTE4:
            return integer ? VK_FORMAT_R8G8B8A8_UINT : VK_FORMAT_R8G8B8A8_USCALED;
        case ElementType::SHORT4:
            return integer ? VK_FORMAT_R16G16B16A16_SINT : VK_FORMAT_R16G16B16A16_SSCALED;
        case ElementType::USHORT4:
            return integer ? VK_FORMAT_R16G16B16A16_UINT : VK_FORMAT_R16G16B16A16_USCALED;
        // case ElementType::HALF4:
        //     return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ElementType::FLOAT4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
    }
    return VK_FORMAT_UNDEFINED;
}

static VkFormat GetVkFormat(PixelDataFormat format, PixelDataType type) {
#define CONVERT(FORMAT, TYPE, VK) \
    if (PixelDataFormat::FORMAT == format && PixelDataType::TYPE == type) return VK_FORMAT_##VK;

    CONVERT(R, UBYTE, R8_UNORM);
    CONVERT(R, BYTE, R8_SNORM);
    CONVERT(RG, UBYTE, R8G8_UNORM);
    CONVERT(RG, BYTE, R8G8_SNORM);
    CONVERT(RGBA, UBYTE, R8G8B8A8_UNORM);
    CONVERT(RGBA, BYTE, R8G8B8A8_SNORM);

    CONVERT(R, FLOAT, R32_SFLOAT);
    CONVERT(RG, FLOAT, R32G32_SFLOAT);
    CONVERT(RGBA, FLOAT, R32G32B32A32_SFLOAT);
#undef CONVERT

    return VK_FORMAT_UNDEFINED;
}

static VkFilter GetFilter(SamplerMinFilter filter) {
    switch (filter) {
        case SamplerMinFilter::NEAREST:
            return VK_FILTER_NEAREST;
        case SamplerMinFilter::LINEAR:
            return VK_FILTER_LINEAR;
        case SamplerMinFilter::NEAREST_MIPMAP_NEAREST:
            return VK_FILTER_NEAREST;
        case SamplerMinFilter::LINEAR_MIPMAP_NEAREST:
            return VK_FILTER_LINEAR;
        case SamplerMinFilter::NEAREST_MIPMAP_LINEAR:
            return VK_FILTER_NEAREST;
        case SamplerMinFilter::LINEAR_MIPMAP_LINEAR:
            return VK_FILTER_LINEAR;
    }
}

static VkFilter GetFilter(SamplerMagFilter filter) {
    switch (filter) {
        case SamplerMagFilter::NEAREST:
            return VK_FILTER_NEAREST;
        case SamplerMagFilter::LINEAR:
            return VK_FILTER_LINEAR;
    }
}

static VkSamplerMipmapMode GetMipmapMode(SamplerMinFilter filter) {
    switch (filter) {
        case SamplerMinFilter::NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case SamplerMinFilter::LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        case SamplerMinFilter::NEAREST_MIPMAP_NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case SamplerMinFilter::LINEAR_MIPMAP_NEAREST:
            return VK_SAMPLER_MIPMAP_MODE_NEAREST;
        case SamplerMinFilter::NEAREST_MIPMAP_LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
        case SamplerMinFilter::LINEAR_MIPMAP_LINEAR:
            return VK_SAMPLER_MIPMAP_MODE_LINEAR;
    }
}

static VkSamplerAddressMode GetWrapMode(SamplerWrapMode mode) {
    switch (mode) {
        case SamplerWrapMode::REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        case SamplerWrapMode::CLAMP_TO_EDGE:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case SamplerWrapMode::MIRRORED_REPEAT:
            return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    }
}

static VkBool32 GetCompareEnable(SamplerCompareMode mode) {
    return mode == SamplerCompareMode::NONE ? VK_FALSE : VK_TRUE;
}

static float GetMaxLod(SamplerMinFilter filter) noexcept {
    switch (filter) {
        case SamplerMinFilter::NEAREST:
        case SamplerMinFilter::LINEAR:
            // The Vulkan spec recommends a max LOD of 0.25 to "disable" mipmapping.
            // See "Mapping of OpenGL to Vulkan filter modes" in the VK Spec.
            return 0.25f;
        case SamplerMinFilter::NEAREST_MIPMAP_NEAREST:
        case SamplerMinFilter::LINEAR_MIPMAP_NEAREST:
        case SamplerMinFilter::NEAREST_MIPMAP_LINEAR:
        case SamplerMinFilter::LINEAR_MIPMAP_LINEAR:
            // Assuming our maximum texture size is 4k, we'll never need more than 12 miplevels.
            return 12.0f;
    }
}

static VkCompareOp GetCompareOp(SamplerCompareFunc func) {
    switch (func) {
        case SamplerCompareFunc::LE:
            return VK_COMPARE_OP_LESS_OR_EQUAL;
        case SamplerCompareFunc::GE:
            return VK_COMPARE_OP_GREATER_OR_EQUAL;
        case SamplerCompareFunc::L:
            return VK_COMPARE_OP_LESS;
        case SamplerCompareFunc::G:
            return VK_COMPARE_OP_GREATER;
        case SamplerCompareFunc::E:
            return VK_COMPARE_OP_EQUAL;
        case SamplerCompareFunc::NE:
            return VK_COMPARE_OP_NOT_EQUAL;
        case SamplerCompareFunc::A:
            return VK_COMPARE_OP_ALWAYS;
        case SamplerCompareFunc::N:
            return VK_COMPARE_OP_NEVER;
    }
}

static VkBufferUsageFlagBits GetBufferObjectUsage(BufferObjectBinding bindingType) noexcept {
    switch (bindingType) {
        case BufferObjectBinding::VERTEX:
            return VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        case BufferObjectBinding::UNIFORM:
            return VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        case BufferObjectBinding::SHADER_STORAGE:
            return VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            // when adding more buffer-types here, make sure to update VulkanBuffer::loadFromCpu()
            // if necessary.
    }
}

}  // namespace VulkanUtils
}  // namespace Backend
}  // namespace Ethereal
bool operator<(const VkImageSubresourceRange& a, const VkImageSubresourceRange& b);
