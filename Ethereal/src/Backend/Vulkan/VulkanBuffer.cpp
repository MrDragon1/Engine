#include "VulkanBuffer.h"

namespace Ethereal {
namespace Backend {

VulkanBuffer::VulkanBuffer(Ref<VulkanContext> context, VkBufferUsageFlags usage, uint32_t numBytes)
    : mContext(context), mUsage(usage) {
    // Create the VkBuffer.
    VkBufferCreateInfo bufferInfo{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                  .size = numBytes,
                                  .usage = usage | VK_BUFFER_USAGE_TRANSFER_DST_BIT};

    VmaAllocationCreateInfo allocInfo{.usage = VMA_MEMORY_USAGE_GPU_ONLY};
    mGpuMemory = VulkanAllocator::AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_GPU_ONLY, mGpuBuffer);
}

void VulkanBuffer::Clear() {
    VulkanAllocator::DestroyBuffer(mGpuBuffer, mGpuMemory);
    mGpuMemory = VK_NULL_HANDLE;
    mGpuBuffer = VK_NULL_HANDLE;
}

void VulkanBuffer::LoadData(const BufferDescriptor& bd, uint32_t byteOffset) {
    // transfer data to gpu buffer
    const VkCommandBuffer cmdbuffer = mContext->mDevice->GetCommandBuffer(true);

    // create staging buffer
    VkBuffer stageBuffer;
    VmaAllocation stageMemory;
    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = bd.size,
        .usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
    };
    stageMemory =
        VulkanAllocator::AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_CPU_ONLY, stageBuffer);

    // copy data to staging buffer
    uint8_t* mappedData;
    mappedData = VulkanAllocator::MapMemory<uint8_t>(stageMemory);
    memcpy(mappedData, bd.buffer, bd.size);
    VulkanAllocator::UnmapMemory(stageMemory);

    VkBufferCopy region{.size = bd.size};
    vkCmdCopyBuffer(cmdbuffer, stageBuffer, mGpuBuffer, 1, &region);

    // Firstly, ensure that the copy finishes before the next draw call.
    // Secondly, in case the user decides to upload another chunk (without ever using the first one)
    // we need to ensure that this upload completes first (hence
    // dstStageMask=VK_PIPELINE_STAGE_TRANSFER_BIT).
    VkAccessFlags dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
    VkPipelineStageFlags dstStageMask = VK_PIPELINE_STAGE_TRANSFER_BIT;
    if (mUsage & VK_BUFFER_USAGE_VERTEX_BUFFER_BIT) {
        dstAccessMask |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
        dstStageMask |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    } else if (mUsage & VK_BUFFER_USAGE_INDEX_BUFFER_BIT) {
        dstAccessMask |= VK_ACCESS_INDEX_READ_BIT;
        dstStageMask |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;
    } else if (mUsage & VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT) {
        dstAccessMask |= VK_ACCESS_UNIFORM_READ_BIT;
        // NOTE: ideally dstStageMask would include VERTEX_SHADER_BIT | FRAGMENT_SHADER_BIT, but
        // this seems to be insufficient on Mali devices. To work around this we are using a more
        // aggressive ALL_GRAPHICS_BIT barrier.
        dstStageMask |= VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT;
    } else if (mUsage & VK_BUFFER_USAGE_STORAGE_BUFFER_BIT) {
        // TODO: implement me
    }

    VkBufferMemoryBarrier barrier{
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
        .dstAccessMask = dstAccessMask,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .buffer = mGpuBuffer,
        .size = VK_WHOLE_SIZE,
    };

    vkCmdPipelineBarrier(cmdbuffer, VK_PIPELINE_STAGE_TRANSFER_BIT, dstStageMask, 0, 0, nullptr, 1,
                         &barrier, 0, nullptr);

    mContext->mDevice->FlushCommandBuffer(cmdbuffer);

    // clean stage buffer
    VulkanAllocator::DestroyBuffer(stageBuffer, stageMemory);
}

}  // namespace Backend
}  // namespace Ethereal
