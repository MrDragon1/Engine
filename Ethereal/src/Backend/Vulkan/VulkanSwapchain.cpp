#include "VulkanSwapchain.h"

#include "Base/Application.h"
#include "Base/GlobalContext.h"

#include "GLFW/glfw3.h"

namespace Ethereal {
namespace Backend {
void VulkanSwapchain::Init(VkInstance instance, Ref<VulkanDevice> device) {
    mInstance = instance;
    mDevice = device;
}

void VulkanSwapchain::InitSurface(void* windowHandle) {
    VkPhysicalDevice physicalDevice = mDevice->GetPhysicalDevice()->GetVulkanPhysicalDevice();

    glfwCreateWindowSurface(mInstance, (GLFWwindow*)windowHandle, nullptr, &mSurface);

    // Get available queue family properties
    uint32_t queueCount;
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, NULL);
    if (queueCount < 1) {
        throw std::runtime_error("VulkanSwapchain::InitSurface: No queue families found.");
    }

    std::vector<VkQueueFamilyProperties> queueProps(queueCount);
    vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueCount, queueProps.data());

    // Iterate over each queue to learn whether it supports presenting:
    // Find a queue with present support
    // Will be used to present the swap chain images to the windowing system
    std::vector<VkBool32> supportsPresent(queueCount);
    for (uint32_t i = 0; i < queueCount; i++) {
        vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, mSurface, &supportsPresent[i]);
    }

    // Search for a graphics and a present queue in the array of queue
    // families, try to find one that supports both
    uint32_t graphicsQueueNodeIndex = UINT32_MAX;
    uint32_t presentQueueNodeIndex = UINT32_MAX;
    for (uint32_t i = 0; i < queueCount; i++) {
        if ((queueProps[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0) {
            if (graphicsQueueNodeIndex == UINT32_MAX) {
                graphicsQueueNodeIndex = i;
            }

            if (supportsPresent[i] == VK_TRUE) {
                graphicsQueueNodeIndex = i;
                presentQueueNodeIndex = i;
                break;
            }
        }
    }
    if (presentQueueNodeIndex == UINT32_MAX) {
        // If there's no queue that supports both present and graphics
        // try to find a separate present queue
        for (uint32_t i = 0; i < queueCount; ++i) {
            if (supportsPresent[i] == VK_TRUE) {
                presentQueueNodeIndex = i;
                break;
            }
        }
    }

    if (graphicsQueueNodeIndex == UINT32_MAX || presentQueueNodeIndex == UINT32_MAX) {
        throw std::runtime_error(
            "VulkanSwapchain::InitSurface: Could not find a graphics and/or presenting queue!");
    }

    mQueueNodeIndex = graphicsQueueNodeIndex;

    FindImageFormatAndColorSpace();
}

void VulkanSwapchain::Create(uint32_t width, uint32_t height) {
    VkDevice device = mDevice->GetDevice();
    VkPhysicalDevice physicalDevice = mDevice->GetPhysicalDevice()->GetVulkanPhysicalDevice();

    VkSwapchainKHR oldSwapchain = mSwapchain;

    // Get physical device surface properties and formats
    VkSurfaceCapabilitiesKHR surfCaps;
    if (vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, mSurface, &surfCaps) !=
        VK_SUCCESS) {
        throw std::runtime_error(
            "VulkanSwapchain::Create: Failed to get physical device surface capabilities.");
    }

    // Get available present modes
    uint32_t presentModeCount;
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, mSurface, &presentModeCount,
                                                  NULL) != VK_SUCCESS) {
        throw std::runtime_error(
            "VulkanSwapchain::Create: Failed to get physical device surface present modes.");
    }
    if (presentModeCount <= 0) {
        throw std::runtime_error(
            "VulkanSwapchain::Create: Failed to get physical device surface present modes.");
    }

    std::vector<VkPresentModeKHR> presentModes(presentModeCount);
    if (vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, mSurface, &presentModeCount,
                                                  presentModes.data()) != VK_SUCCESS) {
        throw std::runtime_error(
            "VulkanSwapchain::Create: Failed to get physical device surface present modes.");
    }

    VkExtent2D swapchainExtent = {};
    // If width (and height) equals the special value 0xFFFFFFFF, the size of the surface will be
    // set by the swapchain
    if (surfCaps.currentExtent.width == (uint32_t)-1) {
        // If the surface size is undefined, the size is set to
        // the size of the images requested.
        swapchainExtent.width = width;
        swapchainExtent.height = height;
    } else {
        // If the surface size is defined, the swap chain size must match
        swapchainExtent = surfCaps.currentExtent;
        width = surfCaps.currentExtent.width;
        height = surfCaps.currentExtent.height;
    }

    mWidth = width;
    mHeight = height;

    // Select a present mode for the swapchain

    // The VK_PRESENT_MODE_FIFO_KHR mode must always be present as per spec
    // This mode waits for the vertical blank ("v-sync")
    VkPresentModeKHR swapchainPresentMode = VK_PRESENT_MODE_FIFO_KHR;

    // If v-sync is not requested, try to find a mailbox mode
    // It's the lowest latency non-tearing present mode available
    if (!Application::Get().GetWindow().IsVSync()) {
        for (size_t i = 0; i < presentModeCount; i++) {
            if (presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR) {
                swapchainPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
            if ((swapchainPresentMode != VK_PRESENT_MODE_MAILBOX_KHR) &&
                (presentModes[i] == VK_PRESENT_MODE_IMMEDIATE_KHR)) {
                swapchainPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
            }
        }
    }

    // Determine the number of images
    uint32_t desiredNumberOfSwapchainImages = surfCaps.minImageCount + 1;
    if ((surfCaps.maxImageCount > 0) && (desiredNumberOfSwapchainImages > surfCaps.maxImageCount)) {
        desiredNumberOfSwapchainImages = surfCaps.maxImageCount;
    }

    // Find the transformation of the surface
    VkSurfaceTransformFlagsKHR preTransform;
    if (surfCaps.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) {
        // We prefer a non-rotated transform
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    } else {
        preTransform = surfCaps.currentTransform;
    }

    // Find a supported composite alpha format (not all devices support alpha opaque)
    VkCompositeAlphaFlagBitsKHR compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    // Simply select the first composite alpha format available
    std::vector<VkCompositeAlphaFlagBitsKHR> compositeAlphaFlags = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };
    for (auto& compositeAlphaFlag : compositeAlphaFlags) {
        if (surfCaps.supportedCompositeAlpha & compositeAlphaFlag) {
            compositeAlpha = compositeAlphaFlag;
            break;
        };
    }

    VkSwapchainCreateInfoKHR swapchainCI = {};
    swapchainCI.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCI.pNext = NULL;
    swapchainCI.surface = mSurface;
    swapchainCI.minImageCount = desiredNumberOfSwapchainImages;
    swapchainCI.imageFormat = mColorFormat;
    swapchainCI.imageColorSpace = mColorSpace;
    swapchainCI.imageExtent = {swapchainExtent.width, swapchainExtent.height};
    swapchainCI.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchainCI.preTransform = (VkSurfaceTransformFlagBitsKHR)preTransform;
    swapchainCI.imageArrayLayers = 1;
    swapchainCI.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    swapchainCI.queueFamilyIndexCount = 0;
    swapchainCI.pQueueFamilyIndices = NULL;
    swapchainCI.presentMode = swapchainPresentMode;
    swapchainCI.oldSwapchain = oldSwapchain;
    // Setting clipped to VK_TRUE allows the implementation to discard rendering outside of the
    // surface area
    swapchainCI.clipped = VK_TRUE;
    swapchainCI.compositeAlpha = compositeAlpha;

    // Enable transfer source on swap chain images if supported
    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT) {
        swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    // Enable transfer destination on swap chain images if supported
    if (surfCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT) {
        swapchainCI.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    if (vkCreateSwapchainKHR(device, &swapchainCI, nullptr, &mSwapchain) != VK_SUCCESS) {
        throw std::runtime_error("VulkanSwapchain::Create: Failed to create swap chain.");
    }

    if (oldSwapchain) vkDestroySwapchainKHR(device, oldSwapchain, nullptr);

    for (auto& image : mImages) vkDestroyImageView(device, image.ImageView, nullptr);
    mImages.clear();

    if (vkGetSwapchainImagesKHR(device, mSwapchain, &mImageCount, NULL) != VK_SUCCESS) {
        throw std::runtime_error("VulkanSwapchain::Create: Failed to get swap chain images.");
    }
    // Get the swap chain images
    mImages.resize(mImageCount);
    mVulkanImages.resize(mImageCount);
    if (vkGetSwapchainImagesKHR(device, mSwapchain, &mImageCount, mVulkanImages.data()) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanSwapchain::Create: Failed to get swap chain images.");
    }

    // Get the swap chain buffers containing the image and imageview
    mImages.resize(mImageCount);
    for (uint32_t i = 0; i < mImageCount; i++) {
        VkImageViewCreateInfo colorAttachmentView = {};
        colorAttachmentView.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorAttachmentView.pNext = NULL;
        colorAttachmentView.format = mColorFormat;
        colorAttachmentView.image = mVulkanImages[i];
        colorAttachmentView.components = {VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                                          VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A};
        colorAttachmentView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorAttachmentView.subresourceRange.baseMipLevel = 0;
        colorAttachmentView.subresourceRange.levelCount = 1;
        colorAttachmentView.subresourceRange.baseArrayLayer = 0;
        colorAttachmentView.subresourceRange.layerCount = 1;
        colorAttachmentView.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorAttachmentView.flags = 0;

        mImages[i].Image = mVulkanImages[i];

        if (vkCreateImageView(device, &colorAttachmentView, nullptr, &mImages[i].ImageView) !=
            VK_SUCCESS) {
            throw std::runtime_error("VulkanSwapchain::Create: Failed to create image view.");
        }
    }

    // Create command buffers
    {
        for (auto& commandBuffer : mCommandBuffers)
            vkDestroyCommandPool(device, commandBuffer.CommandPool, nullptr);

        VkCommandPoolCreateInfo cmdPoolInfo = {};
        cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        cmdPoolInfo.queueFamilyIndex = mQueueNodeIndex;
        cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1;

        mCommandBuffers.resize(mImageCount);
        for (auto& commandBuffer : mCommandBuffers) {
            if (vkCreateCommandPool(device, &cmdPoolInfo, nullptr, &commandBuffer.CommandPool) !=
                VK_SUCCESS) {
                throw std::runtime_error("VulkanSwapchain::Create: Failed to create command pool.");
            }

            commandBufferAllocateInfo.commandPool = commandBuffer.CommandPool;
            if (vkAllocateCommandBuffers(device, &commandBufferAllocateInfo,
                                         &commandBuffer.CommandBuffer) != VK_SUCCESS) {
                throw std::runtime_error(
                    "VulkanSwapchain::Create: Failed to allocate command buffer.");
            }
        }
    }

    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Synchronization Objects
    /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (!mSemaphores.RenderComplete || !mSemaphores.PresentComplete) {
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        if (vkCreateSemaphore(mDevice->GetDevice(), &semaphoreCreateInfo, nullptr,
                              &mSemaphores.RenderComplete) != VK_SUCCESS) {
            throw std::runtime_error("VulkanSwapchain::Create: Failed to create semaphore.");
        }
        if (vkCreateSemaphore(mDevice->GetDevice(), &semaphoreCreateInfo, nullptr,
                              &mSemaphores.PresentComplete) != VK_SUCCESS) {
            throw std::runtime_error("VulkanSwapchain::Create: Failed to create semaphore.");
        }
    }

    if (mWaitFences.size() != mImageCount) {
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        mWaitFences.resize(mImageCount);
        for (auto& fence : mWaitFences) {
            if (vkCreateFence(mDevice->GetDevice(), &fenceCreateInfo, nullptr, &fence) !=
                VK_SUCCESS) {
                throw std::runtime_error("VulkanSwapchain::Create: Failed to create fence.");
            }
        }
    }

    VkPipelineStageFlags pipelineStageFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    mSubmitInfo = {};
    mSubmitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    mSubmitInfo.pWaitDstStageMask = &pipelineStageFlags;
    mSubmitInfo.waitSemaphoreCount = 1;
    mSubmitInfo.pWaitSemaphores = &mSemaphores.PresentComplete;
    mSubmitInfo.signalSemaphoreCount = 1;
    mSubmitInfo.pSignalSemaphores = &mSemaphores.RenderComplete;

    VkFormat depthFormat = mDevice->GetPhysicalDevice()->GetDepthFormat();

    // Render Pass
    VkAttachmentDescription colorAttachmentDesc = {};
    // Color attachment
    colorAttachmentDesc.format = mColorFormat;
    colorAttachmentDesc.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentDesc.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    colorAttachmentDesc.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDesc.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDesc.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDesc.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDesc.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorReference = {};
    colorReference.attachment = 0;
    colorReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    VkAttachmentReference depthReference = {};
    depthReference.attachment = 1;
    depthReference.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;
    subpassDescription.pResolveAttachments = nullptr;

    VkSubpassDependency dependency = {};
    dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass = 0;
    dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependency.srcAccessMask = 0;
    dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.pAttachments = &colorAttachmentDesc;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = 1;
    renderPassInfo.pDependencies = &dependency;

    if (vkCreateRenderPass(mDevice->GetDevice(), &renderPassInfo, nullptr, &mRenderPass) !=
        VK_SUCCESS) {
        throw std::runtime_error("VulkanSwapchain::Create: Failed to create render pass.");
    }

    // Create framebuffers for every swapchain image
    {
        for (auto& framebuffer : mFramebuffers) vkDestroyFramebuffer(device, framebuffer, nullptr);

        VkFramebufferCreateInfo frameBufferCreateInfo = {};
        frameBufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        frameBufferCreateInfo.renderPass = mRenderPass;
        frameBufferCreateInfo.attachmentCount = 1;
        frameBufferCreateInfo.width = mWidth;
        frameBufferCreateInfo.height = mHeight;
        frameBufferCreateInfo.layers = 1;

        mFramebuffers.resize(mImageCount);
        for (uint32_t i = 0; i < mFramebuffers.size(); i++) {
            frameBufferCreateInfo.pAttachments = &mImages[i].ImageView;
            if (vkCreateFramebuffer(mDevice->GetDevice(), &frameBufferCreateInfo, nullptr,
                                    &mFramebuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("VulkanSwapchain::Create: Failed to create framebuffer.");
            }
        }
    }
}

void VulkanSwapchain::Clean() {
    auto device = mDevice->GetDevice();
    vkDeviceWaitIdle(device);

    if (mSwapchain) vkDestroySwapchainKHR(device, mSwapchain, nullptr);

    for (auto& image : mImages) vkDestroyImageView(device, image.ImageView, nullptr);

    for (auto& commandBuffer : mCommandBuffers)
        vkDestroyCommandPool(device, commandBuffer.CommandPool, nullptr);

    if (mRenderPass) vkDestroyRenderPass(device, mRenderPass, nullptr);

    for (auto framebuffer : mFramebuffers) vkDestroyFramebuffer(device, framebuffer, nullptr);

    if (mSemaphores.RenderComplete) vkDestroySemaphore(device, mSemaphores.RenderComplete, nullptr);

    if (mSemaphores.PresentComplete)
        vkDestroySemaphore(device, mSemaphores.PresentComplete, nullptr);

    for (auto& fence : mWaitFences) vkDestroyFence(device, fence, nullptr);

    if (mSurface) vkDestroySurfaceKHR(mInstance, mSurface, nullptr);

    vkDeviceWaitIdle(device);
}

void VulkanSwapchain::OnResize(uint32_t width, uint32_t height) {
    auto device = mDevice->GetDevice();
    vkDeviceWaitIdle(device);
    Create(width, height);
    vkDeviceWaitIdle(device);
}

uint32_t VulkanSwapchain::AcquireNextImage() {
    uint32_t imageIndex;
    if (vkAcquireNextImageKHR(mDevice->GetDevice(), mSwapchain, UINT64_MAX,
                              mSemaphores.PresentComplete, VK_NULL_HANDLE,
                              &imageIndex) != VK_SUCCESS) {
        throw std::runtime_error("Failed to acquire next image");
    }
    return imageIndex;
}

void VulkanSwapchain::FindImageFormatAndColorSpace() {
    VkPhysicalDevice physicalDevice = mDevice->GetPhysicalDevice()->GetVulkanPhysicalDevice();

    // Get list of supported surface formats
    uint32_t formatCount;
    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mSurface, &formatCount, NULL) !=
        VK_SUCCESS) {
        throw std::runtime_error("Failed to get physical device surface formats");
    }
    if (formatCount <= 0) {
        throw std::runtime_error("No physical device surface formats found");
    }
    std::vector<VkSurfaceFormatKHR> surfaceFormats(formatCount);

    if (vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, mSurface, &formatCount,
                                             surfaceFormats.data()) != VK_SUCCESS) {
        throw std::runtime_error("Failed to get physical device surface formats");
    }

    // If the surface format list only includes one entry with VK_FORMAT_UNDEFINED,
    // there is no preferred format, so we assume VK_FORMAT_B8G8R8A8_UNORM
    if ((formatCount == 1) && (surfaceFormats[0].format == VK_FORMAT_UNDEFINED)) {
        mColorFormat = VK_FORMAT_B8G8R8A8_UNORM;
        mColorSpace = surfaceFormats[0].colorSpace;
    } else {
        // iterate over the list of available surface format and
        // check for the presence of VK_FORMAT_B8G8R8A8_UNORM
        bool found_B8G8R8A8_UNORM = false;
        for (auto&& surfaceFormat : surfaceFormats) {
            if (surfaceFormat.format == VK_FORMAT_B8G8R8A8_UNORM) {
                mColorFormat = surfaceFormat.format;
                mColorSpace = surfaceFormat.colorSpace;
                found_B8G8R8A8_UNORM = true;
                break;
            }
        }

        // in case VK_FORMAT_B8G8R8A8_UNORM is not available
        // select the first available color format
        if (!found_B8G8R8A8_UNORM) {
            mColorFormat = surfaceFormats[0].format;
            mColorSpace = surfaceFormats[0].colorSpace;
        }
    }
}

}  // namespace Backend
}  // namespace Ethereal
