#include "VulkanDevice.h"

namespace Ethereal {
namespace Backend {
VulkanPhysicalDevice::VulkanPhysicalDevice() {}

VulkanPhysicalDevice::~VulkanPhysicalDevice() { Clean(); }

void VulkanPhysicalDevice::Init(VkInstance instance) {
    // Get physical device
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
    if (deviceCount == 0) throw(false, "Failed to find GPUs with Vulkan support!");
    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

    // Select physical device
    for (const auto& device : devices) {
        if (IsDeviceSuitable(device)) {
            mPhysicalDevice = device;
            break;
        }
    }

    if (mPhysicalDevice == VK_NULL_HANDLE) throw(false, "Failed to find a suitable GPU!");

    // Get device properties
    vkGetPhysicalDeviceProperties(mPhysicalDevice, &mProperties);
    vkGetPhysicalDeviceFeatures(mPhysicalDevice, &mFeatures);
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);

    // Get queue family properties
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount, nullptr);
    assert(queueFamilyCount > 0);
    mQueueFamilyProperties.resize(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(mPhysicalDevice, &queueFamilyCount,
                                             mQueueFamilyProperties.data());

    // Get list of supported extensions
    uint32_t extensionCount = 0;
    vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount, nullptr);
    if (extensionCount > 0) {
        std::vector<VkExtensionProperties> extensions(extensionCount);
        if (vkEnumerateDeviceExtensionProperties(mPhysicalDevice, nullptr, &extensionCount,
                                                 &extensions.front()) == VK_SUCCESS) {
            for (auto& ext : extensions) {
                mSupportedExtensions.insert(ext.extensionName);
            }
        }
    }

    // Gather physical device memory properties
    vkGetPhysicalDeviceMemoryProperties(mPhysicalDevice, &mMemoryProperties);

    static const float defaultQueuePriority(0.0f);
    // Get queue family indices for graphics and compute
    mQueueFamilyIndices = GetQueueFamilyIndices(VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT);

    // Graphics queue
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.queueFamilyIndex = mQueueFamilyIndices.Graphics;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &defaultQueuePriority;
    mQueueCreateInfos.push_back(queueInfo);

    // Compute queue
    if (mQueueFamilyIndices.Compute != mQueueFamilyIndices.Graphics) {
        VkDeviceQueueCreateInfo queueInfo{};
        queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueInfo.queueFamilyIndex = mQueueFamilyIndices.Compute;
        queueInfo.queueCount = 1;
        queueInfo.pQueuePriorities = &defaultQueuePriority;
        mQueueCreateInfos.push_back(queueInfo);
    }

    mDepthFormat = GetDepthFormat();
}

void VulkanPhysicalDevice::Clean() {}

bool VulkanPhysicalDevice::IsExtensionSupported(const std::string& extensionName) {
    return mSupportedExtensions.find(extensionName) != mSupportedExtensions.end();
}

uint32_t VulkanPhysicalDevice::GetMemoryTypeIndex(uint32_t typeBits,
                                                  VkMemoryPropertyFlags properties) {
    // Iterate over all memory types available for the device used in this example
    for (uint32_t i = 0; i < mMemoryProperties.memoryTypeCount; i++) {
        if ((typeBits & 1) == 1) {
            if ((mMemoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
                return i;
        }
        typeBits >>= 1;
    }

    throw(false, "Could not find a suitable memory type!");
    return UINT32_MAX;
}

VkFormat VulkanPhysicalDevice::FindDepthFormat() {  // Since all depth formats may be optional, we
                                                    // need to find a suitable depth format to use
                                                    // Start with the highest precision packed
                                                    // format
    std::vector<VkFormat> depthFormats = {VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D32_SFLOAT,
                                          VK_FORMAT_D24_UNORM_S8_UINT, VK_FORMAT_D16_UNORM_S8_UINT,
                                          VK_FORMAT_D16_UNORM};

    // TODO: Move to VulkanPhysicalDevice
    for (auto& format : depthFormats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(mPhysicalDevice, format, &formatProps);
        // Format must support depth stencil attachment for optimal tiling
        if (formatProps.optimalTilingFeatures & VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT)
            return format;
    }
    return VK_FORMAT_UNDEFINED;
}

VulkanPhysicalDevice::QueueFamilyIndices VulkanPhysicalDevice::GetQueueFamilyIndices(int flags) {
    QueueFamilyIndices indices;

    // Dedicated queue for compute
    // Try to find a queue family index that supports compute but not graphics
    if (flags & VK_QUEUE_COMPUTE_BIT) {
        for (uint32_t i = 0; i < mQueueFamilyProperties.size(); i++) {
            auto& queueFamilyProperties = mQueueFamilyProperties[i];
            if ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) &&
                ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)) {
                indices.Compute = i;
                break;
            }
        }
    }

    // Dedicated queue for transfer
    // Try to find a queue family index that supports transfer but not graphics and compute
    if (flags & VK_QUEUE_TRANSFER_BIT) {
        for (uint32_t i = 0; i < mQueueFamilyProperties.size(); i++) {
            auto& queueFamilyProperties = mQueueFamilyProperties[i];
            if ((queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT) &&
                ((queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) &&
                ((queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)) {
                indices.Transfer = i;
                break;
            }
        }
    }

    // For other queue types or if no separate compute queue is present, return the first one to
    // support the requested flags
    for (uint32_t i = 0; i < mQueueFamilyProperties.size(); i++) {
        if ((flags & VK_QUEUE_TRANSFER_BIT) && indices.Transfer == -1) {
            if (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_TRANSFER_BIT) indices.Transfer = i;
        }

        if ((flags & VK_QUEUE_COMPUTE_BIT) && indices.Compute == -1) {
            if (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) indices.Compute = i;
        }

        if (flags & VK_QUEUE_GRAPHICS_BIT) {
            if (mQueueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) indices.Graphics = i;
        }
    }

    return indices;
}

bool VulkanPhysicalDevice::IsDeviceSuitable(VkPhysicalDevice device) {
    vkGetPhysicalDeviceProperties(device, &mProperties);
    if (mProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU) {
        return true;
    }
    return false;
}

/////////////////////////////////////////////////////////
///////////////////// VulkanDevice //////////////////////
/////////////////////////////////////////////////////////

VulkanDevice::VulkanDevice() {}

VulkanDevice::~VulkanDevice() {}

void VulkanDevice::Init(VkInstance instance) {
    mPhysicalDevice = Ref<VulkanPhysicalDevice>::Create();
    mPhysicalDevice->Init(instance);

    // Create logical device
    std::vector<const char*> deviceExtensions;
    deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

    VkDeviceCreateInfo deviceCreateInfo{};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount =
        static_cast<uint32_t>(mPhysicalDevice->mQueueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = mPhysicalDevice->mQueueCreateInfos.data();
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

    if (vkCreateDevice(mPhysicalDevice->mPhysicalDevice, &deviceCreateInfo, nullptr, &mDevice) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to create logical device!");
    }

    // Create command pool
    VkCommandPoolCreateInfo cmdPoolInfo{};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = mPhysicalDevice->mQueueFamilyIndices.Graphics;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(mDevice, &cmdPoolInfo, nullptr, &mCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create command pool!");
    }

    cmdPoolInfo.queueFamilyIndex = mPhysicalDevice->mQueueFamilyIndices.Compute;
    if (vkCreateCommandPool(mDevice, &cmdPoolInfo, nullptr, &mComputeCommandPool) != VK_SUCCESS) {
        throw std::runtime_error("Failed to create compute command pool!");
    }

    // Get a graphics queue from the device
    vkGetDeviceQueue(mDevice, mPhysicalDevice->mQueueFamilyIndices.Graphics, 0, &mGraphicsQueue);

    // Get a compute queue from the device
    vkGetDeviceQueue(mDevice, mPhysicalDevice->mQueueFamilyIndices.Compute, 0, &mComputeQueue);
}

void VulkanDevice::Clean() {
    vkDestroyCommandPool(mDevice, mCommandPool, nullptr);
    vkDestroyCommandPool(mDevice, mComputeCommandPool, nullptr);

    vkDeviceWaitIdle(mDevice);
    vkDestroyDevice(mDevice, nullptr);
}

VkCommandBuffer VulkanDevice::CreateSecondaryCommandBuffer(const string& debugName) {
    VkCommandBuffer cmdBuffer;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = mCommandPool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
    cmdBufAllocateInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(mDevice, &cmdBufAllocateInfo, &cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }
    return cmdBuffer;
}

VkCommandBuffer VulkanDevice::GetCommandBuffer(bool begin, bool compute /*= false*/) {
    VkCommandBuffer cmdBuffer;

    VkCommandBufferAllocateInfo cmdBufAllocateInfo = {};
    cmdBufAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmdBufAllocateInfo.commandPool = compute ? mComputeCommandPool : mCommandPool;
    cmdBufAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmdBufAllocateInfo.commandBufferCount = 1;

    if (vkAllocateCommandBuffers(mDevice, &cmdBufAllocateInfo, &cmdBuffer) != VK_SUCCESS) {
        throw std::runtime_error("Failed to allocate command buffer!");
    }

    // If requested, also start the new command buffer
    if (begin) {
        VkCommandBufferBeginInfo cmdBufferBeginInfo{};
        cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        if (vkBeginCommandBuffer(cmdBuffer, &cmdBufferBeginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin command buffer!");
        }
    }

    return cmdBuffer;
}

void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer) {
    FlushCommandBuffer(commandBuffer, mGraphicsQueue);
}

void VulkanDevice::FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue) {
    const uint64_t DEFAULT_FENCE_TIMEOUT = 100000000000;

    if (commandBuffer == VK_NULL_HANDLE) {
        throw("Cannot flush null command buffer");
    }

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw("Failed to end command buffer");
    }

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    // Create fence to ensure that the command buffer has finished executing
    VkFenceCreateInfo fenceCreateInfo = {};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = 0;
    VkFence fence;
    if (vkCreateFence(mDevice, &fenceCreateInfo, nullptr, &fence) != VK_SUCCESS) {
        throw("Failed to create fence");
    }

    // Submit to the queue
    if (vkQueueSubmit(queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw("Failed to submit to queue");
    }
    // Wait for the fence to signal that command buffer has finished executing
    if (vkWaitForFences(mDevice, 1, &fence, VK_TRUE, DEFAULT_FENCE_TIMEOUT) != VK_SUCCESS) {
        throw("Failed to wait for fence");
    }

    vkDestroyFence(mDevice, fence, nullptr);
    vkFreeCommandBuffers(mDevice, mCommandPool, 1, &commandBuffer);
}
}  // namespace Backend
}  // namespace Ethereal
