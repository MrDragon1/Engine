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

    struct alignas(8) FramebufferKey {
        VkRenderPass renderPass;                                 // 8 bytes
        uint16_t width;                                          // 2 bytes
        uint16_t height;                                         // 2 bytes
        uint16_t layers;                                         // 2 bytes
        uint16_t samples;                                        // 2 bytes
        VkImageView color[MAX_SUPPORTED_RENDER_TARGET_COUNT];    // 64 bytes
        VkImageView resolve[MAX_SUPPORTED_RENDER_TARGET_COUNT];  // 64 bytes
        VkImageView depth;                                       // 8 bytes
    };
    struct FramebufferVal {
        VkFramebuffer framebuffer;
    };
    static_assert(sizeof(VkRenderPass) == 8, "VkRenderPass has unexpected size.");
    static_assert(sizeof(VkImageView) == 8, "VkImageView has unexpected size.");
    static_assert(sizeof(FramebufferKey) == 152, "FboKey has unexpected size.");
    using FboKeyHashFn = Ethereal::Math::Hash::MurmurHashFn<FramebufferKey>;
    struct FboKeyEqualFn {
        bool operator()(const FramebufferKey& k1, const FramebufferKey& k2) const;
    };

    void Init(VkDevice device);
    VkRenderPass GetRenderPass(RenderPassKey config);
    VkFramebuffer GetFramebuffer(FramebufferKey config);

   private:
    VkDevice mDevice;
    unordered_map<RenderPassKey, RenderPassVal, RenderPassHash, RenderPassEq> mRenderPassCache;
    unordered_map<FramebufferKey, FramebufferVal, FboKeyHashFn, FboKeyEqualFn> mFramebufferCache;
};
}  // namespace Backend
}  // namespace Ethereal
