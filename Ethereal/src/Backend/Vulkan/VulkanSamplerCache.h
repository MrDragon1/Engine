#pragma once
#include "Backend/Vulkan/VulkanUtils.h"
#include "Backend/DriverBase.h"
namespace Ethereal {
namespace Backend {
class VulkanSamplerGroup : public SamplerGroup {
   public:
    VulkanSamplerGroup(uint32_t){};  // TODO: Implement
};

class VulkanSamplerCache : public RefCounted {
   public:
    void Init(VkDevice device);
    VkSampler GetSampler(SamplerParams params);
    void Reset();

   private:
    VkDevice mDevice;
    unordered_map<uint32_t, VkSampler> mCache;
};
}  // namespace Backend
}  // namespace Ethereal
