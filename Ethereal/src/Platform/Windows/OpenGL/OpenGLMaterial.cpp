#include "OpenGLMaterial.h"

namespace Ethereal
{
    OpenGLMaterial::OpenGLMaterial(const std::string& name) : m_Name(name) {
        m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
        m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
    }
    OpenGLMaterial::OpenGLMaterial(Ref<Material> material, const std::string& name) : m_Name(name) {
        if (name.empty()) m_Name = material->GetName();
        auto openGLMaterial = material.As<OpenGLMaterial>();
        m_Textures = openGLMaterial->m_Textures;
        m_TextureArrays = openGLMaterial->m_TextureArrays;
    }

    OpenGLMaterial::~OpenGLMaterial() {}
}  // namespace Ethereal