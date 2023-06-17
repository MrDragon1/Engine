#include "VulkanDriverApi.h"

namespace Ethereal {
namespace Backend {
VulkanDriverApi::VulkanDriverApi() { Init(); }

VulkanDriverApi::~VulkanDriverApi() { Clean(); }

void VulkanDriverApi::Init() {
    mContext = Ref<VulkanContext>::Create();
    mContext->Init();
}

void VulkanDriverApi::Clean() { mContext->Clean(); }

}  // namespace Backend
}  // namespace Ethereal
