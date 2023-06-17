#pragma once
#include "Backend/Vulkan/VulkanDevice.h"
namespace Ethereal {
namespace Backend {
class VulkanSwapchain : public RefCounted {
   public:
    void Init(VkInstance instance, Ref<VulkanDevice> device);
    void InitSurface(void* windowHandle);
    void Create(uint32_t width, uint32_t height);
    void Clean();

    void OnResize(uint32_t width, uint32_t height);

    uint32_t GetImageCount() const { return mImageCount; }

    uint32_t GetWidth() const { return mWidth; }
    uint32_t GetHeight() const { return mHeight; }

    VkRenderPass GetRenderPass() { return mRenderPass; }

    VkFramebuffer GetCurrentFramebuffer() { return GetFramebuffer(mCurrentImageIndex); }
    VkCommandBuffer GetCurrentDrawCommandBuffer() {
        return GetDrawCommandBuffer(mCurrentBufferIndex);
    }

    VkFormat GetColorFormat() { return mColorFormat; }

    uint32_t GetCurrentBufferIndex() const { return mCurrentBufferIndex; }

    VkFramebuffer GetFramebuffer(uint32_t index) {
        if (index > mFramebuffers.size()) {
            return nullptr;
        }
        return mFramebuffers[index];
    }

    VkCommandBuffer GetDrawCommandBuffer(uint32_t index) {
        if (index > mCommandBuffers.size()) {
            return nullptr;
        }
        return mCommandBuffers[index].CommandBuffer;
    }

    VkSemaphore GetRenderCompleteSemaphore() { return mSemaphores.RenderComplete; }

    uint32_t AcquireNextImage();

    void Present();
    void ResetCommandPool();

   private:
    void FindImageFormatAndColorSpace();

   private:
    VkInstance mInstance = nullptr;
    Ref<VulkanDevice> mDevice;

    VkFormat mColorFormat;
    VkColorSpaceKHR mColorSpace;

    VkSwapchainKHR mSwapchain = nullptr;
    uint32_t mImageCount = 0;
    std::vector<VkImage> mVulkanImages;

    struct SwapchainImage {
        VkImage Image = nullptr;
        VkImageView ImageView = nullptr;
    };
    std::vector<SwapchainImage> mImages;

    struct {
        VkImage Image = nullptr;
        VmaAllocation MemoryAlloc = nullptr;
        VkImageView ImageView = nullptr;
    } mDepthStencil;

    std::vector<VkFramebuffer> mFramebuffers;

    struct SwapchainCommandBuffer {
        VkCommandPool CommandPool = nullptr;
        VkCommandBuffer CommandBuffer = nullptr;
    };
    std::vector<SwapchainCommandBuffer> mCommandBuffers;

    struct {
        // Swap chain
        VkSemaphore PresentComplete = nullptr;
        // Command buffer
        VkSemaphore RenderComplete = nullptr;
    } mSemaphores;
    VkSubmitInfo mSubmitInfo;

    std::vector<VkFence> mWaitFences;

    VkRenderPass mRenderPass = nullptr;
    uint32_t mCurrentBufferIndex = 0;
    uint32_t mCurrentImageIndex = 0;

    uint32_t mQueueNodeIndex = UINT32_MAX;
    uint32_t mWidth = 0, mHeight = 0;

    VkSurfaceKHR mSurface;

    friend class VulkanContext;
    friend class VulkanDriverApi;
};
}  // namespace Backend
}  // namespace Ethereal
