#pragma once
#include "Backend/Vulkan/VulkanUtils.h"
#include "Backend/Vulkan/VulkanDevice.h"
namespace Ethereal {
namespace Backend {
class VulkanAllocator {
   public:
    VulkanAllocator() = default;
    ~VulkanAllocator() = default;
    template <typename T>
    T* MapMemory(VmaAllocation allocation) {
        T* mappedMemory;
        vmaMapMemory(VulkanAllocator::GetAllocator(), allocation, (void**)&mappedMemory);
        return mappedMemory;
    }

    void UnmapMemory(VmaAllocation allocation);

    static void Init(VkInstance, Ref<VulkanDevice>);
    static void Clean();

    static VmaAllocator GetAllocator();

   private:
};
}  // namespace Backend
}  // namespace Ethereal
