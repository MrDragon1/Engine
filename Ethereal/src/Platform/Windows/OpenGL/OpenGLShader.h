#pragma once
#include "Base/Math/Vector.h"
#include "Core/Renderer/Shader.h"
#include <glm/glm.hpp>
#include <unordered_map>

// TODO: REMOVE!
typedef unsigned int GLenum;
namespace Ethereal
{
    class OpenGLShader : public Shader {
      public:
        OpenGLShader(const std::string& filepath);
        OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const std::string& geometrySrc = "");
        virtual ~OpenGLShader();

        virtual void Bind() const override;
        virtual void Unbind() const override;

        virtual void SetFloat(const std::string& name, float value) override;
        virtual void SetFloat3(const std::string& name, const Vector3& value) override;
        virtual void SetFloat4(const std::string& name, const Vector4& value) override;
        virtual void SetMat4(const std::string& name, const Matrix4& value) override;
        virtual void SetInt(const std::string& name, int value) override;
        virtual void SetIntArray(const std::string& name, int* values, uint32_t count) override;

        void UploadUniformInt(const std::string& name, int value);
        void UploadUniformIntArray(const std::string& name, int* values, uint32_t count);

        void UploadUniformFloat(const std::string& name, float value);
        void UploadUniformFloat2(const std::string& name, const Vector2& value);
        void UploadUniformFloat3(const std::string& name, const Vector3& value);
        void UploadUniformFloat4(const std::string& name, const Vector4& value);

        void UploadUniformMat3(const std::string& name, const Matrix3& matrix);
        void UploadUniformMat4(const std::string& name, const Matrix4& matrix);

        virtual const std::string& GetName() const override { return m_Name; };

      private:
        std::string ReadFile(const std::string& filepath);
        std::unordered_map<GLenum, std::string> PreProcess(const std::string& source);
        void Compile(const std::unordered_map<GLenum, std::string>& shaderSources);

      private:
        uint32_t m_RendererID;
        std::string m_Name;
    };
}  // namespace Ethereal