#include "pch.h"
#include "Shader.h"

#include "RendererAPI.h"
#include "Platform/Windows/OpenGL/OpenGLShader.h"

#define LOAD_SHADER_VERT_FRAG(name) ETHEREAL_SHADER_PACK name##ShaderPack{{ETHEREAL_SHADER_TYPE::VERTEX, name##_VERT}, {ETHEREAL_SHADER_TYPE::FRAGMENT, name##_FRAG}}; \
                            Load(#name, name##ShaderPack);
#define LOAD_SHADER_VERT_GEOM_FRAG(name) ETHEREAL_SHADER_PACK name##ShaderPack{{ETHEREAL_SHADER_TYPE::VERTEX, name##_VERT}, {ETHEREAL_SHADER_TYPE::GEOMETRY, name##_GEOM}, {ETHEREAL_SHADER_TYPE::FRAGMENT, name##_FRAG}}; \
                            Load(#name, name##ShaderPack);

namespace Ethereal
{
    Ref<Shader> Shader::Create(const std::string& name, const ETHEREAL_SHADER_PACK& shaderCode) {
        switch (RendererAPI::GetAPI()) {
            case RendererAPI::API::None:
                ET_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
                return nullptr;
            case RendererAPI::API::OpenGL:
                return Ref<OpenGLShader>::Create(name, shaderCode);
        }

        ET_CORE_ASSERT(false, "Unknown RendererAPI!");
        return nullptr;
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader) {
        ET_CORE_ASSERT(!Exists(name), "Shader already exists!");
        m_Shaders[name] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const ETHEREAL_SHADER_PACK& shaderCode) {
        auto shader = Shader::Create(name, shaderCode);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name) {
        ET_CORE_ASSERT(Exists(name), "Shader not found!");
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const { return m_Shaders.find(name) != m_Shaders.end(); }

    void ShaderLibrary::Init() {
        // Basics
        LOAD_SHADER_VERT_FRAG(PBR)
        LOAD_SHADER_VERT_FRAG(PBRANIM)
        LOAD_SHADER_VERT_FRAG(SKYBOX)

        // Bloom
        LOAD_SHADER_VERT_FRAG(BLUR)
        LOAD_SHADER_VERT_FRAG(BRIGHT)
        LOAD_SHADER_VERT_FRAG(MERGE)

        // CSM
        LOAD_SHADER_VERT_GEOM_FRAG(CSM)
        LOAD_SHADER_VERT_FRAG(SHADOWMAP)

        // IBL
        LOAD_SHADER_VERT_FRAG(PREFILTER)
        LOAD_SHADER_VERT_FRAG(IRRADIANCECONVOLUTION)
        LOAD_SHADER_VERT_FRAG(EQUIRECTANGULARTOCUBEMAP)
    }
}  // namespace Ethereal