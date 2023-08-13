#pragma once
#include "Backend/DriverBase.h"
#include "Backend/Vulkan/VulkanBuffer.h"
#include "Backend/Vulkan/VulkanTexture.h"
// Derived class from DriverBase

namespace Ethereal {
namespace Backend {

struct VulkanBufferObject : public BufferObject {
    VulkanBufferObject(Ref<VulkanContext> context, uint32_t byteCount,
                       BufferObjectBinding bindingType, BufferUsage usage);
    void Clear() { buffer->Clear(); }
    Ref<VulkanBuffer> buffer;
    const BufferObjectBinding bindingType;
};

struct VulkanVertexBuffer : public VertexBuffer {
    VulkanVertexBuffer(Ref<VulkanContext> context, const AttributeArray& attributeArray,
                       uint32_t vertexCount, uint8_t attributeCount, uint8_t bufferCount);
    vector<Ref<VulkanBuffer>> buffers;
};

struct VulkanIndexBuffer : public IndexBuffer {
    VulkanIndexBuffer(Ref<VulkanContext> context, uint8_t elementSize, uint32_t indexCount);
    void Clear() { buffer->Clear(); }
    Ref<VulkanBuffer> buffer;
    const VkIndexType indexType;
};

struct VulkanRenderPrimitive : public RenderPrimitive {
    void SetPrimitiveType(PrimitiveType pt);
    void SetBuffers(Ref<VulkanVertexBuffer> vertexBuffer, Ref<VulkanIndexBuffer> indexBuffer);
    VkPrimitiveTopology primitiveTopology;
};

struct VulkanProgram : public Program {
    VulkanProgram(Ref<VulkanContext> context, std::string_view name, ShaderSource source);
    VkShaderModule vertexShaderModule;
    VkShaderModule fragmentShaderModule;
    VkSpecializationInfo* specializationInfos = nullptr;

   private:
    VkDevice device;
};

struct VulkanAttachment {
    Ref<VulkanTexture> texture;
    uint32_t level = 0;
    uint32_t layer = 0;

    VkImage GetImage();
    VkFormat GetFormat();
    VulkanLayout GetLayout();
    VkExtent2D GetExtent2D();
    VkImageView GetImageView(VkImageAspectFlags aspect = VK_IMAGE_ASPECT_COLOR_BIT);
    VkImageSubresourceRange GetSubresourceRange(VkImageAspectFlags aspect);
};
struct VulkanRenderTarget : public RenderTarget {
    VulkanRenderTarget(Ref<VulkanContext> context, uint32_t width, uint32_t height,
                       VulkanAttachment color[MAX_SUPPORTED_RENDER_TARGET_COUNT],
                       VulkanAttachment depthStencil[2]);
    VkExtent2D GetExtent();
    VulkanAttachment GetColor(int target);
    VulkanAttachment GetDepth();
    bool HasDepth() const { return depth.texture; }
    VulkanAttachment color[MAX_SUPPORTED_RENDER_TARGET_COUNT] = {};
    VulkanAttachment depth = {};
};

}  // namespace Backend
}  // namespace Ethereal
