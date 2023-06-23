#pragma once
#include "Backend/Vulkan/VulkanUtils.h"
#include "Base/Math/Hash.h"
namespace Ethereal {
namespace Backend {
class VulkanFramebufferCache : public RefCounted {
   public:
    struct alignas(8) RenderPassKey {
        VulkanLayout initialDepthLayout : 4;
        VulkanLayout renderPassDepthLayout : 4;
        VulkanLayout finalDepthLayout : 4;
        uint8_t padding0 : 4;

        VkFormat colorFormat[MAX_SUPPORTED_RENDER_TARGET_COUNT];
        VkFormat depthFormat;

        TargetBufferFlags clearMask;
        TargetBufferFlags discardStart;
        TargetBufferFlags discardEnd;
    };
    struct RenderPassVal {
        VkRenderPass renderPass;
    };

    static_assert(0 == MAX_SUPPORTED_RENDER_TARGET_COUNT % 8);
    static_assert(sizeof(TargetBufferFlags) == 4, "TargetBufferFlags has unexpected size.");
    static_assert(sizeof(VkFormat) == 4, "VkFormat has unexpected size.");
    static_assert(sizeof(RenderPassKey) == 56, "RenderPassKey has unexpected size.");
    using RenderPassHash = Ethereal::Math::Hash::MurmurHashFn<RenderPassKey>;
    struct RenderPassEq {
        bool operator()(const RenderPassKey& k1, const RenderPassKey& k2) const;
    };

    void Init(VkDevice device);
    VkRenderPass GetRenderPass(RenderPassKey config);

   private:
    VkDevice mDevice;
    unordered_map<RenderPassKey, RenderPassVal, RenderPassHash, RenderPassEq> mRenderPassCache;
};
}  // namespace Backend
}  // namespace Ethereal
