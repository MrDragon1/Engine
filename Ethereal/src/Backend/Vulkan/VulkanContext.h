#pragma once
#include "Backend/Vulkan/VulkanDevice.h"
#include "Backend/Vulkan/VulkanSwapchain.h"
#include "Backend/Vulkan/VulkanSamplerCache.h"
namespace Ethereal {
namespace Backend {
class VulkanContext : public RefCounted {
   public:
    void Init();
    void Clean();
    VkInstance GetInstance() { return mInstance; }

   private:
    void CreateInstance();
    void CreateDevice();
    void SetupDebugMessenger();

    std::vector<const char*> GetRequiredExtensions();
    bool CheckValidationLayerSupport();
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);

    static VKAPI_ATTR VkBool32 VKAPI_CALL
    DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                  VkDebugUtilsMessageTypeFlagsEXT messageType,
                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

   public:
    VkInstance mInstance;

    Ref<VulkanDevice> mDevice;
    VkDebugUtilsMessengerEXT debugMessenger;

    Ref<VulkanSwapchain> mSwapchain;

    Ref<VulkanSamplerCache> mSamplerCache;
};
}  // namespace Backend
}  // namespace Ethereal
