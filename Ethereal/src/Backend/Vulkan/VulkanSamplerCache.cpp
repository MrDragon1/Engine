#include "VulkanSamplerCache.h"

namespace Ethereal {
namespace Backend {
void VulkanSamplerCache::Init(VkDevice device) { mDevice = device; }

VkSampler VulkanSamplerCache::GetSampler(SamplerParams params) {
    auto iter = mCache.find(params.u);
    if (iter != mCache.end()) {
        return iter->second;
    }
    VkSamplerCreateInfo samplerInfo{
        .sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
        .magFilter = VulkanUtils::GetFilter(params.filterMag),
        .minFilter = VulkanUtils::GetFilter(params.filterMin),
        .mipmapMode = VulkanUtils::GetMipmapMode(params.filterMin),
        .addressModeU = VulkanUtils::GetWrapMode(params.wrapS),
        .addressModeV = VulkanUtils::GetWrapMode(params.wrapT),
        .addressModeW = VulkanUtils::GetWrapMode(params.wrapR),
        .anisotropyEnable = params.anisotropyLog2 == 0 ? 0u : 1u,
        .maxAnisotropy = (float)(1u << params.anisotropyLog2),
        .compareEnable = VulkanUtils::GetCompareEnable(params.compareMode),
        .compareOp = VulkanUtils::GetCompareOp(params.compareFunc),
        .minLod = 0.0f,
        .maxLod = VulkanUtils::GetMaxLod(params.filterMin),
        .borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
        .unnormalizedCoordinates = VK_FALSE};
    VkSampler sampler;
    VkResult error = vkCreateSampler(mDevice, &samplerInfo, nullptr, &sampler);
    ET_CORE_ASSERT(!error, "Unable to create sampler.");
    mCache.insert({params.u, sampler});
    return sampler;
}

void VulkanSamplerCache::Reset() {
    for (auto pair : mCache) {
        vkDestroySampler(mDevice, pair.second, nullptr);
    }
    mCache.clear();
}

}  // namespace Backend
}  // namespace Ethereal
