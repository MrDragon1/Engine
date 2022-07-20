#include "EnvironmentMapRenderPass.hpp"

namespace Ethereal
{

    void EnvironmentMapRenderPass::Init(uint32_t width, uint32_t height) {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_PLACEHOLDER, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = width;
        fbSpec.Height = height;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_EquirectangularToCubeMapShader = Shader::Create(m_EquirectangularToCubeMapShaderPath);
        m_EquirectangularToCubeMapShader->Bind();

        m_IrradianceConvolutionShader = Shader::Create(m_IrradianceConvolutionShaderPath);
        m_IrradianceConvolutionShader->Bind();

        Ref<TextureData> data512 = CreateRef<TextureData>();
        data512->m_width = 512;
        data512->m_height = 512;
        m_EnvCubeMap = TextureCube::Create(data512);
        Ref<TextureData> data32 = CreateRef<TextureData>();
        data32->m_width = 32;
        data32->m_height = 32;
        m_IrradianceCubeMap = TextureCube::Create(data32);

        m_HDRTexture = Texture2D::Create(m_HDRTexturePath);
    }

    void EnvironmentMapRenderPass::Draw() {
        if (!m_IsFirstCall) return;

        glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
        glm::mat4 captureViews[] = {glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
                                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)),
                                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)),
                                    glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f))};

        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LEQUAL);
        m_EquirectangularToCubeMapShader->Bind();
        m_EquirectangularToCubeMapShader->SetInt("u_EquirectangularMap", 0);
        m_EquirectangularToCubeMapShader->SetMat4("u_Projection", captureProjection);

        m_Framebuffer->Resize(512, 512);
        m_Framebuffer->Bind();

        for (int i = 0; i < 6; i++) {
            m_HDRTexture->Bind(0);
            m_EquirectangularToCubeMapShader->SetMat4("u_View", captureViews[i]);
            m_EnvCubeMap->BindToFramebuffer(i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube.m_VAO, m_Cube.m_IndexCount);
        }

        m_IrradianceConvolutionShader->Bind();
        m_IrradianceConvolutionShader->SetInt("u_EnvironmentMap", 0);
        m_IrradianceConvolutionShader->SetMat4("u_Projection", captureProjection);

        m_Framebuffer->Resize(32, 32);
        m_Framebuffer->Bind();

        for (int i = 0; i < 6; i++) {
            m_EnvCubeMap->Bind(0);
            m_IrradianceConvolutionShader->SetMat4("u_View", captureViews[i]);
            m_IrradianceCubeMap->BindToFramebuffer(i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube.m_VAO, m_Cube.m_IndexCount);
        }

        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LESS);
        m_IsFirstCall = false;
    }

    void EnvironmentMapRenderPass::OnResize(uint32_t width, uint32_t height) {}
}  // namespace Ethereal