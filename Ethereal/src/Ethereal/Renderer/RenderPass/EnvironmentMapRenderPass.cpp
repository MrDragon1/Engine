#include "EnvironmentMapRenderPass.hpp"
#include "Ethereal/Utils/AssetManager.hpp"

namespace Ethereal
{

    void EnvironmentMapRenderPass::Init(uint32_t width, uint32_t height) {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_PLACEHOLDER, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_PLACEHOLDER,
                              ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = width;
        fbSpec.Height = height;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_EquirectangularToCubeMapShader = Shader::Create(m_EquirectangularToCubeMapShaderPath);
        m_EquirectangularToCubeMapShader->Bind();
        m_PrefilterShader = Shader::Create(m_PrefilterShaderPath);
        m_PrefilterShader->Bind();

        Ref<TextureData> data512 = CreateRef<TextureData>();
        data512->m_width = 512;
        data512->m_height = 512;
        m_EnvCubeMap = TextureCube::Create(data512);
        Ref<TextureData> data32 = CreateRef<TextureData>();
        data32->m_width = 32;
        data32->m_height = 32;
        m_IrradianceCubeMap = TextureCube::Create(data32);

        Ref<TextureData> data128 = CreateRef<TextureData>();
        data128->m_width = 128;
        data128->m_height = 128;
        m_PrefilterCubeMap = TextureCube::Create(data128);
        m_PrefilterCubeMap->GenerateMipmaps();

        m_EnvTexture = TextureManager::AddTexture(m_EnvTexturePath);
        m_IrradianceTexture = TextureManager::AddTexture(m_IrradianceTexturePath);
        m_BRDFLUTTexture = TextureManager::AddTexture(m_BRDFLUTTexturePath);
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
        m_EquirectangularToCubeMapShader->SetInt("u_EquirectangularMap0", 0);
        m_EquirectangularToCubeMapShader->SetMat4("u_Projection", captureProjection);

        // Generate Environment CubeMap
        m_Framebuffer->Resize(512, 512);
        m_Framebuffer->Bind();
        for (int i = 0; i < 6; i++) {
            m_EnvTexture->Bind(0);
            m_EquirectangularToCubeMapShader->SetMat4("u_View", captureViews[i]);
            m_EnvCubeMap->BindToFramebuffer(0, i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube.m_VAO, m_Cube.m_IndexCount);
        }

        // Generate Irradiance CubeMap
        m_Framebuffer->Resize(32, 32);
        m_Framebuffer->Bind();
        for (int i = 0; i < 6; i++) {
            m_IrradianceTexture->Bind(0);
            m_EquirectangularToCubeMapShader->SetMat4("u_View", captureViews[i]);
            m_IrradianceCubeMap->BindToFramebuffer(0, i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube.m_VAO, m_Cube.m_IndexCount);
        }

        // Generate Prefilter CubeMap
        m_PrefilterShader->Bind();
        m_PrefilterShader->SetInt("u_EnvironmentMap", 0);
        m_PrefilterShader->SetMat4("u_Projection", captureProjection);

        m_Framebuffer->Resize(128, 128);
        m_Framebuffer->Bind();
        unsigned int maxMipLevels = 5;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = static_cast<unsigned int>(128 * std::pow(0.5, mip));
            unsigned int mipHeight = static_cast<unsigned int>(128 * std::pow(0.5, mip));

            //RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);
            m_Framebuffer->Resize(mipWidth, mipHeight);
            m_Framebuffer->Bind();
            float roughness = (float)mip / (float)(maxMipLevels - 1);
            m_PrefilterShader->SetFloat("u_Roughness", roughness);
            for (int i = 0; i < 6; i++) {
                m_EnvCubeMap->Bind(0);
                m_PrefilterShader->SetMat4("u_View", captureViews[i]);
                m_PrefilterCubeMap->BindToFramebuffer(0, i, mip);
                RenderCommand::Clear();
                RenderCommand::DrawIndexed(m_Cube.m_VAO, m_Cube.m_IndexCount);
            }
        }
        m_Framebuffer->Unbind();
        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LESS);
        m_IsFirstCall = false;
    }

    void EnvironmentMapRenderPass::OnResize(uint32_t width, uint32_t height) {}
}  // namespace Ethereal