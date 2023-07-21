#include "VulkanFramebufferCache.h"

namespace Ethereal {
namespace Backend {
bool VulkanFramebufferCache::RenderPassEq::operator()(const RenderPassKey& k1,
                                                      const RenderPassKey& k2) const {
    if (k1.initialDepthLayout != k2.initialDepthLayout) return false;
    if (k1.renderPassDepthLayout != k2.renderPassDepthLayout) return false;
    if (k1.finalDepthLayout != k2.finalDepthLayout) return false;
    for (int i = 0; i < MAX_SUPPORTED_RENDER_TARGET_COUNT; i++) {
        if (k1.colorFormat[i] != k2.colorFormat[i]) return false;
    }
    if (k1.depthFormat != k2.depthFormat) return false;
    if (k1.clearMask != k2.clearMask) return false;
    if (k1.discardStart != k2.discardStart) return false;
    if (k1.discardEnd != k2.discardEnd) return false;
    return true;
}

bool VulkanFramebufferCache::FboKeyEqualFn::operator()(const FramebufferKey& k1,
                                                       const FramebufferKey& k2) const {
    if (k1.renderPass != k2.renderPass) return false;
    if (k1.width != k2.width) return false;
    if (k1.height != k2.height) return false;
    if (k1.layers != k2.layers) return false;
    if (k1.samples != k2.samples) return false;
    if (k1.depth != k2.depth) return false;
    for (int i = 0; i < MAX_SUPPORTED_RENDER_TARGET_COUNT; i++) {
        if (k1.color[i] != k2.color[i]) return false;
        if (k1.resolve[i] != k2.resolve[i]) return false;
    }
    return true;
}

void VulkanFramebufferCache::Init(VkDevice device) { mDevice = device; }

VkRenderPass VulkanFramebufferCache::GetRenderPass(RenderPassKey config) {
    auto iter = mRenderPassCache.find(config);
    if (iter != mRenderPassCache.end() && iter->second.renderPass != VK_NULL_HANDLE) {
        return iter->second.renderPass;
    }
    const bool hasSubpasses = false;

    const VkAttachmentLoadOp kClear = VK_ATTACHMENT_LOAD_OP_CLEAR;
    const VkAttachmentLoadOp kDontCare = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    const VkAttachmentLoadOp kKeep = VK_ATTACHMENT_LOAD_OP_LOAD;
    const VkAttachmentStoreOp kDisableStore = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    const VkAttachmentStoreOp kEnableStore = VK_ATTACHMENT_STORE_OP_STORE;

    VkAttachmentReference inputAttachmentRef[MAX_SUPPORTED_RENDER_TARGET_COUNT] = {};
    VkAttachmentReference colorAttachmentRefs[2][MAX_SUPPORTED_RENDER_TARGET_COUNT] = {};
    VkAttachmentReference depthAttachmentRef = {};

    const bool hasDepth = config.depthFormat != VK_FORMAT_UNDEFINED;

    VkSubpassDescription subpasses[2] = {
        {.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
         .pInputAttachments = nullptr,
         .pColorAttachments = colorAttachmentRefs[0],
         .pDepthStencilAttachment = hasDepth ? &depthAttachmentRef : nullptr},
        {.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
         .pInputAttachments = inputAttachmentRef,
         .pColorAttachments = colorAttachmentRefs[1],
         .pDepthStencilAttachment = hasDepth ? &depthAttachmentRef : nullptr}};

    VkAttachmentDescription
        attachments[MAX_SUPPORTED_RENDER_TARGET_COUNT + MAX_SUPPORTED_RENDER_TARGET_COUNT + 1] = {};

    // We support 2 subpasses, which means we need to supply 1 dependency struct.
    VkSubpassDependency dependencies[1] = {{
        .srcSubpass = 0,
        .dstSubpass = 1,
        .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        .dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        .srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
        .dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
        .dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT,
    }};

    VkRenderPassCreateInfo renderPassInfo{.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
                                          .attachmentCount = 0u,
                                          .pAttachments = attachments,
                                          .subpassCount = hasSubpasses ? 2u : 1u,
                                          .pSubpasses = subpasses,
                                          .dependencyCount = hasSubpasses ? 1u : 0u,
                                          .pDependencies = dependencies};

    int attachmentIndex = 0;

    // Populate the Color Attachments.
    for (int i = 0; i < MAX_SUPPORTED_RENDER_TARGET_COUNT; i++) {
        if (config.colorFormat[i] == VK_FORMAT_UNDEFINED) {
            continue;
        }
        const VkImageLayout subpassLayout =
            VulkanUtils::GetVkImageLayout(VulkanLayout::COLOR_ATTACHMENT);
        uint32_t index;

        if (!hasSubpasses) {
            index = subpasses[0].colorAttachmentCount++;
            colorAttachmentRefs[0][index].layout = subpassLayout;
            colorAttachmentRefs[0][index].attachment = attachmentIndex;
        }
        const TargetBufferFlags flag = TargetBufferFlags(int(TargetBufferFlags::COLOR0) << i);
        const bool clear = any(config.clearMask & flag);
        const bool discard = any(config.discardStart & flag);

        attachments[attachmentIndex++] = {
            .format = config.colorFormat[i],
            .samples = (VkSampleCountFlagBits)1,
            .loadOp = clear ? kClear : (discard ? kDontCare : kKeep),
            .storeOp = kEnableStore,
            .stencilLoadOp = kDontCare,
            .stencilStoreOp = kDisableStore,
            .initialLayout = ((!discard && false) || clear)
                                 ? VulkanUtils::GetVkImageLayout(VulkanLayout::COLOR_ATTACHMENT)
                                 : VulkanUtils::GetVkImageLayout(VulkanLayout::UNDEFINED),
            .finalLayout = VulkanUtils::GetVkImageLayout(VulkanLayout::COLOR_ATTACHMENT),
        };
    }

    if (subpasses[0].colorAttachmentCount == 0) {
        subpasses[0].pColorAttachments = nullptr;
        subpasses[0].pResolveAttachments = nullptr;
        subpasses[1].pColorAttachments = nullptr;
        subpasses[1].pResolveAttachments = nullptr;
    }

    // Populate the Depth Attachment.
    if (hasDepth) {
        const bool clear = any(config.clearMask & TargetBufferFlags::DEPTH);
        const bool discardStart = any(config.discardStart & TargetBufferFlags::DEPTH);
        const bool discardEnd = any(config.discardEnd & TargetBufferFlags::DEPTH);
        depthAttachmentRef.layout = VulkanUtils::GetVkImageLayout(config.renderPassDepthLayout);
        depthAttachmentRef.attachment = attachmentIndex;
        attachments[attachmentIndex++] = {
            .format = config.depthFormat,
            .samples = (VkSampleCountFlagBits)1,
            .loadOp = clear ? kClear : (discardStart ? kDontCare : kKeep),
            .storeOp = discardEnd ? kDisableStore : kEnableStore,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VulkanUtils::GetVkImageLayout(config.initialDepthLayout),
            .finalLayout = VulkanUtils::GetVkImageLayout(config.finalDepthLayout),
        };
    }
    renderPassInfo.attachmentCount = attachmentIndex;

    // Finally, create the VkRenderPass.
    VkRenderPass renderPass;
    VkResult error = vkCreateRenderPass(mDevice, &renderPassInfo, nullptr, &renderPass);
    ET_CORE_ASSERT(!error, "Unable to create render pass.");
    mRenderPassCache[config] = {renderPass};

    return renderPass;
}

VkFramebuffer VulkanFramebufferCache::GetFramebuffer(FramebufferKey config) {
    auto iter = mFramebufferCache.find(config);
    if (iter != mFramebufferCache.end()) {
        return iter->second.framebuffer;
    }

    VkImageView
        attachments[MAX_SUPPORTED_RENDER_TARGET_COUNT + MAX_SUPPORTED_RENDER_TARGET_COUNT + 1];
    uint32_t attachmentCount = 0;
    for (VkImageView attachment : config.color) {
        if (attachment) {
            attachments[attachmentCount++] = attachment;
        }
    }
    for (VkImageView attachment : config.resolve) {
        if (attachment) {
            attachments[attachmentCount++] = attachment;
        }
    }
    if (config.depth) {
        attachments[attachmentCount++] = config.depth;
    }

    VkFramebuffer framebuffer;
    VkFramebufferCreateInfo framebufferInfo{.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
                                            .renderPass = config.renderPass,
                                            .attachmentCount = attachmentCount,
                                            .pAttachments = attachments,
                                            .width = config.width,
                                            .height = config.height,
                                            .layers = config.layers};
    VkResult error = vkCreateFramebuffer(mDevice, &framebufferInfo, nullptr, &framebuffer);
    ET_CORE_ASSERT(!error, "Unable to create framebuffer.");
    mFramebufferCache[config] = {framebuffer};

    return framebuffer;
}

}  // namespace Backend
}  // namespace Ethereal
