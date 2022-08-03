#include "pch.h"
#include "Material.h"
#include "Platform/Windows/OpenGL/OpenGLMaterial.h"

#include "RendererAPI.h"
namespace Ethereal
{
    Ref<Material> Material::Create(const Ref<Shader>& shader, const std::string& name) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return Ref<OpenGLMaterial>::Create(shader, name);
        }
        ET_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }

    Ref<Material> Material::Copy(const Ref<Material>& other, const std::string& name) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                return nullptr;
            case RendererAPI::API::OpenGL:
                return Ref<OpenGLMaterial>::Create(other, name);
        }
        ET_CORE_ASSERT(false, "Unknown RendererAPI");
        return nullptr;
    }
}  // namespace Ethereal