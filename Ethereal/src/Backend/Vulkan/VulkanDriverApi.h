#pragma once
#include "Backend/DriverApi.h"
#include "Backend/Vulkan/VulkanContext.h"

namespace Ethereal {
namespace Backend {
class VulkanDriverApi : public DriverApi {
   public:
    VulkanDriverApi();
    ~VulkanDriverApi();
    void Init();
    void Clean();

   private:
    Ref<VulkanContext> mContext;
};
}  // namespace Backend
}  // namespace Ethereal