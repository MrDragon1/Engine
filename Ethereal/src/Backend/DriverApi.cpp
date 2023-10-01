#include "DriverApi.h"

#include "Backend/OpenGL/OpenGLDriverApi.h"
#include "Backend/Vulkan/VulkanDriverApi.h"
#include "Core/Material/MaterialBase/Value.h"

namespace Ethereal {
namespace Backend {

Ref<DriverApi> DriverApi::CreateApi(BackendType backend) {
    switch (backend) {
        case BackendType::NONE:
            ET_CORE_WARN("Backend is not specificated!");
        case BackendType::OPENGL:
            return Ref<OpenGLDriverApi>::Create();
        case BackendType::VULKAN:
            return Ref<VulkanDriverApi>::Create();
    }
    return Ref<OpenGLDriverApi>::Create();
}

}  // namespace Backend
}  // namespace Ethereal