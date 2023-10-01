#pragma once
#include "Backend/DriverBase.h"
#include "Backend/Vulkan/VulkanContext.h"
#include "Backend/Vulkan/VulkanAllocator.h"
#include "Backend/BufferDescriptor.h"
namespace Ethereal {
namespace Backend {
class VulkanBuffer : public RefCounted {
   public:
    VulkanBuffer(Ref<VulkanContext> context, VkBufferUsageFlags usage, uint32_t numBytes);

    void Clear();
    void LoadData(const BufferDescriptor& bd, uint32_t byteOffset);
    VkBuffer GetBuffer() const { return mGpuBuffer; }

   private:
    Ref<VulkanContext> mContext;

    VmaAllocation mGpuMemory = VK_NULL_HANDLE;
    VkBuffer mGpuBuffer = VK_NULL_HANDLE;
    VkBufferUsageFlags mUsage = {};
};

}  // namespace Backend
}  // namespace Ethereal
