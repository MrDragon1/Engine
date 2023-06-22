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

VmaAllocation VulkanAllocator::AllocateBuffer(VkBufferCreateInfo bufferCreateInfo,
                                              VmaMemoryUsage usage, VkBuffer& outBuffer) {
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = usage;

    VmaAllocation allocation;
    vmaCreateBuffer(sData->Allocator, &bufferCreateInfo, &allocCreateInfo, &outBuffer, &allocation,
                    nullptr);

    // TODO: Tracking
    VmaAllocationInfo allocInfo;
    vmaGetAllocationInfo(sData->Allocator, allocation, &allocInfo);

    { sData->TotalAllocatedBytes += allocInfo.size; }

    return allocation;
}

VmaAllocation VulkanAllocator::AllocateImage(VkImageCreateInfo imageCreateInfo,
                                             VmaMemoryUsage usage, VkImage& outImage) {
    VmaAllocationCreateInfo allocCreateInfo = {};
    allocCreateInfo.usage = usage;

    VmaAllocation allocation;
    vmaCreateImage(sData->Allocator, &imageCreateInfo, &allocCreateInfo, &outImage, &allocation,
                   nullptr);

    // TODO: Tracking
    VmaAllocationInfo allocInfo;
    vmaGetAllocationInfo(sData->Allocator, allocation, &allocInfo);
    { sData->TotalAllocatedBytes += allocInfo.size; }
    return allocation;
}

void VulkanAllocator::Free(VmaAllocation allocation) {
    vmaFreeMemory(sData->Allocator, allocation);
}

void VulkanAllocator::DestroyImage(VkImage image, VmaAllocation allocation) {
    vmaDestroyImage(sData->Allocator, image, allocation);
}

void VulkanAllocator::DestroyBuffer(VkBuffer buffer, VmaAllocation allocation) {
    vmaDestroyBuffer(sData->Allocator, buffer, allocation);
}

void VulkanAllocator::UnmapMemory(VmaAllocation allocation) {
    vmaUnmapMemory(sData->Allocator, allocation);
}

void VulkanAllocator::Init(VkInstance instance, Ref<VulkanDevice> device) {
    sData = new VulkanAllocatorData();

    // Initialize VulkanMemoryAllocator
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion =
        VK_API_VERSION_1_3;  // should same with vulkan version in VulkanContext
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
