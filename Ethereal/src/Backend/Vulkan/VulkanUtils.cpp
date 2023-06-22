#include "VulkanUtils.h"

bool operator<(const VkImageSubresourceRange& a, const VkImageSubresourceRange& b) {
    if (a.aspectMask < b.aspectMask) return true;
    if (a.aspectMask > b.aspectMask) return false;
    if (a.baseMipLevel < b.baseMipLevel) return true;
    if (a.baseMipLevel > b.baseMipLevel) return false;
    if (a.levelCount < b.levelCount) return true;
    if (a.levelCount > b.levelCount) return false;
    if (a.baseArrayLayer < b.baseArrayLayer) return true;
    if (a.baseArrayLayer > b.baseArrayLayer) return false;
    if (a.layerCount < b.layerCount) return true;
    if (a.layerCount > b.layerCount) return false;
    return false;
}

namespace Ethereal {
namespace Backend {}
}  // namespace Ethereal
