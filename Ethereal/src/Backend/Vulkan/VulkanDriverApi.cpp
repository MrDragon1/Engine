#include "VulkanDriverApi.h"
#include "Core/Material/MaterialBase/Value.h"

namespace Ethereal {
namespace Backend {
VulkanDriverApi::VulkanDriverApi() { Init(); }

VulkanDriverApi::~VulkanDriverApi() { Clean(); }

void VulkanDriverApi::Init() {
    mContext = Ref<VulkanContext>::Create();
    mContext->Init();
}

void VulkanDriverApi::Clean() { mContext->Clean(); }

void VulkanDriverApi::BeginFrame() {
    mContext->mSwapchain->AcquireNextImage();

    mContext->mSwapchain->ResetCommandPool();
}

void VulkanDriverApi::EndFrame() { mContext->mSwapchain->Present(); }

}  // namespace Backend
}  // namespace Ethereal
