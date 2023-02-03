#include "pch.h"
#include "OpenGLShader.h"

#include <fstream>
#include <glad/glad.h>
#include <gl/glcorearb.h>
#include <glm/gtc/type_ptr.hpp>

namespace Ethereal
{
    static GLenum ToGLShaderType(const ETHEREAL_SHADER_TYPE& type) {
        if (type == ETHEREAL_SHADER_TYPE::VERTEX) return GL_VERTEX_SHADER;
        if (type == ETHEREAL_SHADER_TYPE::FRAGMENT) return GL_FRAGMENT_SHADER;
        if (type == ETHEREAL_SHADER_TYPE::GEOMETRY) return GL_GEOMETRY_SHADER;

        ET_CORE_ASSERT(false, "Unknown shader type!");
        return 0;
    }

    OpenGLShader::OpenGLShader(const std::string& name, const ETHEREAL_SHADER_PACK& shaderCode) {

        Compile(shaderCode);

        m_Name = name;
    }


    OpenGLShader::~OpenGLShader() { glDeleteProgram(m_RendererID); }

    void OpenGLShader::Bind() const { glUseProgram(m_RendererID); }

    void OpenGLShader::Unbind() const { glUseProgram(0); }

    void OpenGLShader::SetFloat(const std::string& name, float value) { UploadUniformFloat(name, value); }

    void OpenGLShader::SetFloat3(const std::string& name, const Vector3& value) { UploadUniformFloat3(name, value); }

    void OpenGLShader::SetFloat4(const std::string& name, const Vector4& value) { UploadUniformFloat4(name, value); }

    void OpenGLShader::SetMat4(const std::string& name, const Matrix4& value) { UploadUniformMat4(name, value); }

    void OpenGLShader::SetInt(const std::string& name, int value) { UploadUniformInt(name, value); }

    void OpenGLShader::SetIntArray(const std::string& name, int* values, uint32_t count) { UploadUniformIntArray(name, values, count); }

    void OpenGLShader::UploadUniformIntArray(const std::string& name, int* values, uint32_t count) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1iv(location, count, values);
    }

    void OpenGLShader::UploadUniformInt(const std::string& name, int value) {
        //TODO: fix wrong location
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1i(location, value);
    }

    void OpenGLShader::UploadUniformFloat(const std::string& name, float value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform1f(location, value);
    }

    void OpenGLShader::UploadUniformFloat2(const std::string& name, const Vector2& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform2f(location, value.x, value.y);
    }

    void OpenGLShader::UploadUniformFloat3(const std::string& name, const Vector3& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform3f(location, value.x, value.y, value.z);
    }

    void OpenGLShader::UploadUniformFloat4(const std::string& name, const Vector4& value) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniform4f(location, value.x, value.y, value.z, value.w);
    }

    void OpenGLShader::UploadUniformMat3(const std::string& name, const Matrix3& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix3fv(location, 1, GL_FALSE, Math::Ptr(matrix));
    }

    void OpenGLShader::UploadUniformMat4(const std::string& name, const Matrix4& matrix) {
        GLint location = glGetUniformLocation(m_RendererID, name.c_str());
        glUniformMatrix4fv(location, 1, GL_FALSE, Math::Ptr(matrix));
    }

    void OpenGLShader::Compile(const ETHEREAL_SHADER_PACK& shaderCode) {
        GLuint program = glCreateProgram();
        std::vector<GLenum> glShaderIDs;
        for (auto& kv : shaderCode) {
            GLenum type = ToGLShaderType(kv.first);
            const std::vector<unsigned char>& source = kv.second;

            GLuint shader = glCreateShader(type);

            glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, source.data(), source.size());
            glSpecializeShader(shader, "main", 0, nullptr, nullptr);

            GLint isCompiled = 0;
            glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
            if (isCompiled == GL_FALSE) {
                GLint maxLength = 0;
                glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

                std::vector<GLchar> infoLog(maxLength);
                glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

                glDeleteShader(shader);

                ET_CORE_ERROR("{0}", infoLog.data());
                ET_CORE_ASSERT(false, "Shader compilation failure!")
                break;
            }

            glAttachShader(program, shader);
            glShaderIDs.push_back(shader);
        }

        // Link our program
        glLinkProgram(program);

        // Note the different functions here: glGetProgram* instead of glGetShader*.
        GLint isLinked = 0;
        glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
        if (isLinked == GL_FALSE) {
            GLint maxLength = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

            // The maxLength includes the NULL character
            std::vector<GLchar> infoLog(maxLength);
            glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

            // We don't need the program anymore.
            glDeleteProgram(program);
            
            for (auto id : glShaderIDs)
                glDeleteShader(id);

            ET_CORE_ERROR("{0}", infoLog.data());
            ET_CORE_ASSERT(false, "OpenGLShader link failure!")
            return;
        }

        for (auto id : glShaderIDs)
            glDetachShader(program, id);

        m_RendererID = program;
    }
}  // namespace Ethereal