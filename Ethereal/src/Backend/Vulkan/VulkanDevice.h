#pragma once
#include "Backend/DriverBase.h"
#include "Backend/Vulkan/VulkanUtils.h"
namespace Ethereal {
namespace Backend {
const std::vector<const char*> validationLayers = {"VK_LAYER_KHRONOS_validation"};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
    VK_EXT_SHADER_VIEWPORT_INDEX_LAYER_EXTENSION_NAME,
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() { return graphicsFamily.has_value() && presentFamily.has_value(); }
};
struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

class VulkanPhysicalDevice : public RefCounted {
   public:
    struct QueueFamilyIndices {
        int32_t Graphics = -1;
        int32_t Compute = -1;
        int32_t Transfer = -1;
    };

   public:
    VulkanPhysicalDevice();
    ~VulkanPhysicalDevice();

    void Init(VkInstance instance);
    void Clean();

    bool IsExtensionSupported(const std::string& extensionName);
    uint32_t GetMemoryTypeIndex(uint32_t typeBits, VkMemoryPropertyFlags properties);

    VkPhysicalDevice GetVulkanPhysicalDevice() { return mPhysicalDevice; }
    const QueueFamilyIndices& GetQueueFamilyIndices() { return mQueueFamilyIndices; }

    const VkPhysicalDeviceProperties& GetProperties() { return mProperties; }
    const VkPhysicalDeviceLimits& GetLimits() { return mProperties.limits; }
    const VkPhysicalDeviceMemoryProperties& GetMemoryProperties() { return mMemoryProperties; }

    VkFormat GetDepthFormat() { return mDepthFormat; }

   private:
    VkFormat FindDepthFormat();
    QueueFamilyIndices GetQueueFamilyIndices(int queueFlags);
    bool IsDeviceSuitable(VkPhysicalDevice);

   private:
    QueueFamilyIndices mQueueFamilyIndices;

    VkPhysicalDevice mPhysicalDevice = nullptr;
    VkPhysicalDeviceProperties mProperties;
    VkPhysicalDeviceFeatures mFeatures;
    VkPhysicalDeviceMemoryProperties mMemoryProperties;

    VkFormat mDepthFormat = VK_FORMAT_UNDEFINED;

    std::vector<VkQueueFamilyProperties> mQueueFamilyProperties;
    std::unordered_set<string> mSupportedExtensions;
    std::vector<VkDeviceQueueCreateInfo> mQueueCreateInfos;

    friend class VulkanDevice;
};

class VulkanDevice : public RefCounted {
   public:
    VulkanDevice();
    ~VulkanDevice();
    void Init(VkInstance instance);
    void Clean();

    Ref<VulkanPhysicalDevice> GetPhysicalDevice() { return mPhysicalDevice; }
    VkDevice GetDevice() { return mDevice; }

    VkCommandPool GetCommandPool() { return mCommandPool; }
    VkCommandPool GetComputeCommandPool() { return mComputeCommandPool; }

    VkQueue GetGraphicsQueue() { return mGraphicsQueue; }
    VkQueue GetComputeQueue() { return mComputeQueue; }

    VkCommandBuffer CreateSecondaryCommandBuffer(const string& debugName);

    VkCommandBuffer GetCommandBuffer(bool begin, bool compute = false);
    void FlushCommandBuffer(VkCommandBuffer commandBuffer);
    void FlushCommandBuffer(VkCommandBuffer commandBuffer, VkQueue queue);
    uint32_t GetMemoryType(uint32_t typeBits, VkMemoryPropertyFlags properties,
                           VkBool32* memTypeFound = nullptr);

   private:
    Ref<VulkanPhysicalDevice> mPhysicalDevice;
    VkDevice mDevice;

    VkPhysicalDeviceFeatures mEnabledFeatures;
    VkCommandPool mCommandPool = nullptr, mComputeCommandPool = nullptr;

    VkQueue mGraphicsQueue;
    VkQueue mComputeQueue;

    VkPhysicalDeviceMemoryProperties memoryProperties;
};
}  // namespace Backend
}  // namespace Ethereal
