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

        Ref<TextureData> data512 = Ref<TextureData>::Create();
        data512->m_width = 512;
        data512->m_height = 512;
        m_BackgroundCubeMap = TextureCube::Create(data512);
        Ref<TextureData> data32 = Ref<TextureData>::Create();
        data32->m_width = 32;
        data32->m_height = 32;
        m_EnvironmentCubeMap = TextureCube::Create(data32);

        Ref<TextureData> data128 = Ref<TextureData>::Create();
        data128->m_width = 128;
        data128->m_height = 128;
        m_ReflectionCubeMap = TextureCube::Create(data128);
        m_ReflectionCubeMap->GenerateMipmaps();

        m_ReflectionCubeMapNoMipmaps = TextureCube::Create(data128);
    }

    void EnvironmentMapRenderPass::Draw() {
        if (!m_IsFirstCall) return;
        ET_CORE_INFO("EnvironmentMapRenderPass::Draw()");
        m_BackgroundTexture = TextureManager::AddTexture(m_BackgroundTexturePath);
        m_EnvironmentTexture = TextureManager::AddTexture(m_EnvironmentTexturePath);
        m_ReflectionTexture = TextureManager::AddTexture(m_ReflectionTexturePath);
        m_BRDFLUTTexture = TextureManager::AddTexture(m_BRDFLUTTexturePath);

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

        // Generate Background CubeMap
        m_Framebuffer->Resize(512, 512);
        m_Framebuffer->Bind();
        for (int i = 0; i < 6; i++) {
            m_BackgroundTexture->Bind(0);
            m_EquirectangularToCubeMapShader->SetMat4("u_View", captureViews[i]);
            m_BackgroundCubeMap->BindToFramebuffer(0, i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube->GetMeshSource()->GetVertexArray(), m_Cube->GetMeshSource()->GetIndexBuffer()->GetCount());
        }

        // Generate Environment CubeMap
        m_Framebuffer->Resize(32, 32);
        m_Framebuffer->Bind();
        for (int i = 0; i < 6; i++) {
            m_EnvironmentTexture->Bind(0);
            m_EquirectangularToCubeMapShader->SetMat4("u_View", captureViews[i]);
            m_EnvironmentCubeMap->BindToFramebuffer(0, i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube->GetMeshSource()->GetVertexArray(), m_Cube->GetMeshSource()->GetIndexBuffer()->GetCount());
        }

        // Generate Reflection CubeMap
        m_Framebuffer->Resize(128, 128);
        m_Framebuffer->Bind();
        for (int i = 0; i < 6; i++) {
            m_ReflectionTexture->Bind(0);
            m_EquirectangularToCubeMapShader->SetMat4("u_View", captureViews[i]);
            m_ReflectionCubeMapNoMipmaps->BindToFramebuffer(0, i);
            RenderCommand::Clear();
            RenderCommand::DrawIndexed(m_Cube->GetMeshSource()->GetVertexArray(), m_Cube->GetMeshSource()->GetIndexBuffer()->GetCount());
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

            // RenderCommand::SetViewport(0, 0, mipWidth, mipHeight);
            m_Framebuffer->Resize(mipWidth, mipHeight);
            m_Framebuffer->Bind();
            float roughness = (float)mip / (float)(maxMipLevels - 1);
            m_PrefilterShader->SetFloat("u_Roughness", roughness);
            for (int i = 0; i < 6; i++) {
                m_ReflectionCubeMapNoMipmaps->Bind(0);
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