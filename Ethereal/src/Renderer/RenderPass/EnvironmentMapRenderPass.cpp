#include "EnvironmentMapRenderPass.h"
#include "Utils/AssetManager.h"
#include "Core/GlobalContext.h"
namespace Ethereal
{
    void EnvironmentMapRenderPass::Init(uint32_t width, uint32_t height) {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_PLACEHOLDER, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_PLACEHOLDER,
                              ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = width;
        fbSpec.Height = height;

        m_Cube = GlobalContext::GetRenderSystem().GetCubeStaticMesh();

        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_EquirectangularToCubeMapShader = Shader::Create(m_EquirectangularToCubeMapShaderPath);
        m_EquirectangularToCubeMapShader->Bind();
        m_PrefilterShader = Shader::Create(m_PrefilterShaderPath);
        m_PrefilterShader->Bind();
        m_ConvolutionShader = Shader::Create(m_ConvolutionShaderPath);
        m_ConvolutionShader->Bind();

        Ref<TextureData> data32 = Ref<TextureData>::Create();
        data32->m_width = 32;
        data32->m_height = 32;
        m_EnvironmentCubeMap = TextureCube::Create(data32);

        Ref<TextureData> data512 = Ref<TextureData>::Create();
        data512->m_width = 512;
        data512->m_height = 512;
        m_ReflectionCubeMap = TextureCube::Create(data512);
        m_ReflectionCubeMap->GenerateMipmaps();

        m_TextureCube = TextureCube::Create(data512);
    }

    void EnvironmentMapRenderPass::Draw() {
        if (!m_IsFirstCall) return;
        ET_CORE_INFO("EnvironmentMapRenderPass::Draw()");
        m_Texture = TextureManager::AddTexture(m_Path);

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
            m_Texture->Bind(0);
            m_EquirectangularToCubeMapShader->SetMat4("u_View", captureViews[i]);
            m_TextureCube->BindToFramebuffer(0, i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube->GetMeshSource()->GetVertexArray(), m_Cube->GetMeshSource()->GetIndexBuffer()->GetCount());
        }

        // Convolution
        m_ConvolutionShader->Bind();
        m_ConvolutionShader->SetInt("u_EnvironmentMap", 0);
        m_ConvolutionShader->SetMat4("u_Projection", captureProjection);

        m_Framebuffer->Resize(32, 32);
        m_Framebuffer->Bind();
        for (int i = 0; i < 6; i++) {
            m_TextureCube->Bind(0);
            m_ConvolutionShader->SetMat4("u_View", captureViews[i]);
            m_EnvironmentCubeMap->BindToFramebuffer(0, i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube->GetMeshSource()->GetVertexArray(), m_Cube->GetMeshSource()->GetIndexBuffer()->GetCount());
        }

        m_EquirectangularToCubeMapShader->Bind();
        m_EquirectangularToCubeMapShader->SetInt("u_EquirectangularMap0", 0);
        m_EquirectangularToCubeMapShader->SetMat4("u_Projection", captureProjection);

        // Generate Reflection CubeMap
        // here, we reuse the m_TextureCube

        // Generate Prefilter CubeMap
        m_PrefilterShader->Bind();
        m_PrefilterShader->SetInt("u_EnvironmentMap", 0);
        m_PrefilterShader->SetMat4("u_Projection", captureProjection);

        m_Framebuffer->Resize(512, 512);
        m_Framebuffer->Bind();
        unsigned int maxMipLevels = 10;
        for (unsigned int mip = 0; mip < maxMipLevels; ++mip) {
            // reisze framebuffer according to mip-level size.
            unsigned int mipWidth = static_cast<unsigned int>(512 * std::pow(0.5, mip));
            unsigned int mipHeight = static_cast<unsigned int>(512 * std::pow(0.5, mip));

            // RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);
            m_Framebuffer->Resize(mipWidth, mipHeight);
            m_Framebuffer->Bind();
            float roughness = (float)mip / (float)(maxMipLevels - 1);
            m_PrefilterShader->SetFloat("u_Roughness", roughness);
            for (int i = 0; i < 6; i++) {
                m_TextureCube->Bind(0);
                m_PrefilterShader->SetMat4("u_View", captureViews[i]);
                m_ReflectionCubeMap->BindToFramebuffer(0, i, mip);
                RenderCommand::Clear();
                RenderCommand::DrawIndexed(m_Cube->GetMeshSource()->GetVertexArray(), m_Cube->GetMeshSource()->GetIndexBuffer()->GetCount());
            }
        }
        m_Framebuffer->Unbind();
        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LESS);
        m_IsFirstCall = false;
    }

    void EnvironmentMapRenderPass::OnResize(uint32_t width, uint32_t height) {}
}  // namespace Ethereal