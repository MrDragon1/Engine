#include "EnvironmentMapRenderPass.h"
#include "Ethereal/src/Base/GlobalContext.h"
namespace Ethereal
{
    void EnvironmentMapRenderPass::Init(uint32_t width, uint32_t height) {
        TextureSpecification spec;
        spec.Format = ETHEREAL_PIXEL_FORMAT::PLACEHOLDER;
        TextureSpecification depthSpec;
        depthSpec.Format = ETHEREAL_PIXEL_FORMAT::DEPTH;

        FramebufferSpecification fbSpec;
        fbSpec.ColorAttachments.PushAttachmentSpec(spec);
        fbSpec.ColorAttachments.PushAttachmentSpec(spec);
        fbSpec.DepthAttachment.SetAttachmentSpec(depthSpec);
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
        ET_CORE_INFO("EnvironmentMapRenderPass::Draw()");
        m_Texture = Texture2D::Create(m_Path);

        Matrix4 captureProjection = Math::Perspective(Math::Radians(90.0f), 1.0f, 0.1f, 10.0f);
        Matrix4 captureViews[] = {Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)),
                                    Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f)),
                                    Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f)),
                                    Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f)),
                                    Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f)),
                                    Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f))};

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
    }

    void EnvironmentMapRenderPass::OnResize(uint32_t width, uint32_t height) {}
}  // namespace Ethereal