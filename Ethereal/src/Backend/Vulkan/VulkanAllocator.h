#pragma once
#include "Backend/Vulkan/VulkanUtils.h"
#include "Backend/Vulkan/VulkanDevice.h"
namespace Ethereal {
namespace Backend {
class VulkanAllocator {
   public:
    VulkanAllocator() = default;
    ~VulkanAllocator() = default;

    static VmaAllocation AllocateBuffer(VkBufferCreateInfo bufferCreateInfo, VmaMemoryUsage usage,
                                        VkBuffer& outBuffer);
    static VmaAllocation AllocateImage(VkImageCreateInfo imageCreateInfo, VmaMemoryUsage usage,
                                       VkImage& outImage);
    static void Free(VmaAllocation allocation);
    static void DestroyImage(VkImage image, VmaAllocation allocation);
    static void DestroyBuffer(VkBuffer buffer, VmaAllocation allocation);

    template <typename T>
    static T* MapMemory(VmaAllocation allocation) {
        T* mappedMemory;
        vmaMapMemory(VulkanAllocator::GetAllocator(), allocation, (void**)&mappedMemory);
        return mappedMemory;
    }

    static void UnmapMemory(VmaAllocation allocation);

    static void Init(VkInstance, Ref<VulkanDevice>);
    static void Clean();

    static VmaAllocator GetAllocator();

   private:
};
}  // namespace Backend
}  // namespace Ethereal
