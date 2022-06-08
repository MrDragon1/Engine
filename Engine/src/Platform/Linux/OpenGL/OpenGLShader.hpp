#pragma once

#include "Core/Renderer/Shader.hpp"
#include <glm/glm.hpp>
#include <unordered_map>

// TODO: REMOVE!
typedef unsigned int GLenum;
namespace Engine
{
    class OpenGLShader : public Shader {
      public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void UploadUniformInt(const std::string& name, int value);

        virtual void UploadUniformFloat(const std::string& name, float value);
        virtual void UploadUniformFloat2(const std::string& name, const glm::vec2& value);
        virtual void UploadUniformFloat3(const std::string& name, const glm::vec3& value);
        virtual void UploadUniformFloat4(const std::string& name, const glm::vec4& value);

        virtual void UploadUniformMat3(const std::string& name, const glm::mat3& matrix);
        virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix);

        virtual const std::string& GetName() const override { return m_Name; };

      private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

      private:
        uint32_t m_RendererID;
        std::string m_Name;
    };
}  // namespace Engine