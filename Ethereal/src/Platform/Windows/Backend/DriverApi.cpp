#include "DriverApi.h"

#include "Platform/Windows/OpenGL/OpenGLDriverApi.h"
namespace Ethereal {
namespace Backend {

Ref<DriverApi> DriverApi::CreateApi(BackendType backend) {
    switch (backend) {
        case BackendType::NONE:
            ET_CORE_WARN("Backend is not specificated!");
        case BackendType::OPENGL:
            return Ref<OpenGLDriverApi>::Create();
    }
    return Ref<OpenGLDriverApi>::Create();
}

}  // namespace Backend
}  // namespace Ethereal