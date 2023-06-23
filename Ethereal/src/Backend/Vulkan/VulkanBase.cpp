#include "VulkanBase.h"

namespace Ethereal {
namespace Backend {
VulkanBufferObject::VulkanBufferObject(Ref<VulkanContext> context, uint32_t byteCount,
                                       BufferObjectBinding bindingType, BufferUsage usage)
    : BufferObject(byteCount), bindingType(bindingType) {
    buffer = Ref<VulkanBuffer>::Create(context, VulkanUtils::GetBufferObjectUsage(bindingType),
                                       byteCount);
}

VulkanVertexBuffer::VulkanVertexBuffer(Ref<VulkanContext> context,
                                       const AttributeArray& attributeArray, uint32_t vertexCount,
                                       uint8_t attributeCount, uint8_t bufferCount)
    : VertexBuffer(attributeArray, vertexCount, attributeCount, bufferCount),
      buffers(bufferCount, nullptr) {}

VulkanIndexBuffer::VulkanIndexBuffer(Ref<VulkanContext> context, uint8_t elementSize,
                                     uint32_t indexCount)
    : IndexBuffer(elementSize, indexCount),
      indexType(elementSize == 2 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32) {
    buffer = Ref<VulkanBuffer>::Create(context, VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                                       elementSize * indexCount);
}

void VulkanRenderPrimitive::SetPrimitiveType(PrimitiveType pt) {
    this->type = pt;
    switch (pt) {
        case PrimitiveType::POINTS:
            primitiveTopology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
            break;
        case PrimitiveType::LINES:
            primitiveTopology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
            break;
        case PrimitiveType::LINE_STRIP:
            primitiveTopology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
            break;
        case PrimitiveType::TRIANGLES:
            primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
            break;
        case PrimitiveType::TRIANGLE_STRIP:
            primitiveTopology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
            break;
    }
}

void VulkanRenderPrimitive::SetBuffers(Ref<VulkanVertexBuffer> vertexBuffer,
                                       Ref<VulkanIndexBuffer> indexBuffer) {
    this->vertexBuffer = vertexBuffer;
    this->indexBuffer = indexBuffer;
}

VulkanProgram::VulkanProgram(Ref<VulkanContext> context, std::string_view name, ShaderSource source)
    : device(context->mDevice->GetDevice()), Program(name) {
    for (auto& kv : source) {
        auto& shaderBlob = kv.second;
        VkShaderModuleCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = shaderBlob.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderBlob.data());

        VkShaderModule shaderModule;
        if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
            ET_CORE_ASSERT("failed to create shader module!");
        }

        if (kv.first == ShaderType::VERTEX) {
            vertexShaderModule = shaderModule;
        } else if (kv.first == ShaderType::FRAGMENT) {
            fragmentShaderModule = shaderModule;
        }
    }
}

VulkanRenderTarget::VulkanRenderTarget(Ref<VulkanContext> context, uint32_t width, uint32_t height,
                                       VulkanAttachment color[MAX_SUPPORTED_RENDER_TARGET_COUNT],
                                       VulkanAttachment depthStencil[2])
    : RenderTarget(width, height) {
    for (int index = 0; index < MAX_SUPPORTED_RENDER_TARGET_COUNT; index++) {
        this->color[index] = color[index];
    }
    this->depth = depthStencil[0];
}

VkExtent2D VulkanRenderTarget::GetExtent() { return {width, height}; }

VulkanAttachment VulkanRenderTarget::GetColor(int target) { return color[target]; }

VulkanAttachment VulkanRenderTarget::GetDepth() { return depth; }

VkImage VulkanAttachment::GetImage() { return texture ? texture->GetVkImage() : VK_NULL_HANDLE; }

VkFormat VulkanAttachment::GetFormat() {
    return texture ? texture->GetVkFormat() : VK_FORMAT_UNDEFINED;
}

VulkanLayout VulkanAttachment::GetLayout() {
    return texture ? texture->GetLayout(layer, level) : VulkanLayout::UNDEFINED;
}

VkExtent2D VulkanAttachment::GetExtent2D() {
    ET_CORE_ASSERT(texture);
    return {std::max(1u, texture->width >> level), std::max(1u, texture->height >> level)};
}

VkImageView VulkanAttachment::GetImageView(VkImageAspectFlags aspect) {
    ET_CORE_ASSERT(texture);
    return texture->GetAttachmentView(GetSubresourceRange(aspect));
}

VkImageSubresourceRange VulkanAttachment::GetSubresourceRange(VkImageAspectFlags aspect) {
    ET_CORE_ASSERT(texture);
    uint32_t levelCount = 1;
    uint32_t layerCount = 1;
    // For depth attachments, we consider all the subresource range since layout transitions of
    // depth and stencil attachments should always be carried out for all subresources.
    if (aspect & VK_IMAGE_ASPECT_DEPTH_BIT) {
        auto range = texture->GetPrimaryRange();
        levelCount = range.levelCount;
        layerCount = range.layerCount;
    }
    return {
        .aspectMask = aspect,
        .baseMipLevel = uint32_t(level),
        .levelCount = levelCount,
        .baseArrayLayer = uint32_t(layer),
        .layerCount = layerCount,
    };
}

}  // namespace Backend
}  // namespace Ethereal
