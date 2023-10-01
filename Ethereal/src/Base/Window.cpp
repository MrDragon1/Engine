#include "Window.h"
#include "Backend/DriverEnum.h"
#include "Backend/OpenGL/GLWindow.h"
#include "Backend/Vulkan/VulkanWindow.h"

namespace Ethereal {
Scope<Window> Window::Create(BackendType backend, const WindowProps& props) {
    switch (backend) {
        case BackendType::NONE:
            ET_CORE_WARN("Backend is not specificated!");
        case BackendType::OPENGL:
            return CreateScope<GLWindow>(props);
        case BackendType::VULKAN:
            return CreateScope<VulkanWindow>(props);
    }
    return CreateScope<GLWindow>(props);
}

}  // namespace Ethereal
