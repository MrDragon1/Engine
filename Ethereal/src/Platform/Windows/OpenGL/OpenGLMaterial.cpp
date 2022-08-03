#include "OpenGLMaterial.h"

namespace Ethereal
{
    OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const std::string& name) : m_Shader(shader), m_Name(name) {
        m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
        m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
    }
}  // namespace Ethereal