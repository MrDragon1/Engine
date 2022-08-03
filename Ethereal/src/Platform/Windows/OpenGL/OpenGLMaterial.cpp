#include "OpenGLMaterial.h"

namespace Ethereal
{
    OpenGLMaterial::OpenGLMaterial(const Ref<Shader>& shader, const std::string& name) : m_Shader(shader), m_Name(name) {
        m_MaterialFlags |= (uint32_t)MaterialFlag::DepthTest;
        m_MaterialFlags |= (uint32_t)MaterialFlag::Blend;
    }
    OpenGLMaterial::OpenGLMaterial(Ref<Material> material, const std::string& name) : m_Shader(material->GetShader()), m_Name(name) {
        if (name.empty()) m_Name = material->GetName();
        auto vulkanMaterial = material.As<OpenGLMaterial>();
        m_Textures = vulkanMaterial->m_Textures;
        m_TextureArrays = vulkanMaterial->m_TextureArrays;
    }

    OpenGLMaterial::~OpenGLMaterial() {}
}  // namespace Ethereal