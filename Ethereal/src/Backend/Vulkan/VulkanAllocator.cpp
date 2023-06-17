#include "VulkanAllocator.h"
#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"
#include "Backend/Vulkan/VulkanContext.h"
namespace Ethereal {
namespace Backend {
struct VulkanAllocatorData {
    VmaAllocator Allocator;
    uint64_t TotalAllocatedBytes = 0;
};

static VulkanAllocatorData* sData = nullptr;

void VulkanAllocator::UnmapMemory(VmaAllocation allocation) {
    vmaUnmapMemory(sData->Allocator, allocation);
}

void VulkanAllocator::Init(VkInstance instance, Ref<VulkanDevice> device) {
    sData = new VulkanAllocatorData();

    // Initialize VulkanMemoryAllocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_2;
    allocatorInfo.physicalDevice = device->GetPhysicalDevice()->GetVulkanPhysicalDevice();
    allocatorInfo.device = device->GetDevice();
    allocatorInfo.instance = instance;

    vmaCreateAllocator(&allocatorInfo, &sData->Allocator);
}

void VulkanAllocator::Clean() {
    vmaDestroyAllocator(sData->Allocator);

    delete sData;
    sData = nullptr;
}

VmaAllocator VulkanAllocator::GetAllocator() { return sData->Allocator; }
}  // namespace Backend
}  // namespace Ethereal
