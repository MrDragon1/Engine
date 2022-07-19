#include "RenderPass.hpp"
#include "RenderCommand.hpp"

namespace Ethereal
{
    VisiableNodes RenderPass::m_VisiableNodes;

    void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
        m_ViewProjectionMatrix = glm::mat4(1.0f);

        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R8G8B8A8_UNORM, ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_R32_INTEGER,
                              ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = width;
        fbSpec.Height = height;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_Shader = Shader::Create(m_ShaderPath);
        m_Shader->Bind();

        m_Shader->SetFloat3("u_Albedo", {0.5f, 0.0f, 0.0f});
        m_Shader->SetFloat("u_Occlusion", 1.0f);

        m_Shader->SetInt("u_AlbedoMap", 0);
        m_Shader->SetInt("u_NormalMap", 1);
        m_Shader->SetInt("u_MetallicMap", 2);
        m_Shader->SetInt("u_RoughnessMap", 3);
        m_Shader->SetInt("u_OcclusionMap", 4);
        m_Shader->SetInt("u_EmissiveMap", 5);
    }

    void MainCameraRenderPass::Draw() {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        const auto& visiableRenderNode = *m_VisiableNodes.p_main_camera_visible_mesh_nodes;
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
        m_Shader->SetMat4("u_LightSpaceMatrix", m_LightSpaceMatrix);
        m_Shader->SetFloat3("camPos", m_CameraPosition);

        // lights
        // ------
        glm::vec3 lightPositions[] = {
            glm::vec3(-10.0f, 20.0f, 10.0f),
            glm::vec3(10.0f, 20.0f, 10.0f),
            glm::vec3(-10.0f, 0.0f, 10.0f),
            glm::vec3(10.0f, 0.0f, 10.0f),
        };
        glm::vec3 lightColors[] = {glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f), glm::vec3(300.0f, 300.0f, 300.0f),
                                   glm::vec3(300.0f, 300.0f, 300.0f)};
        for (int i = 0; i < 4; i++) {
            m_Shader->SetFloat3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
            m_Shader->SetFloat3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
        }

        if (!visiableRenderNode.empty()) {
            for (auto& RenderNode : visiableRenderNode) {
                RenderNode.ref_mesh->m_VAO->Bind();
                m_Shader->SetMat4("u_Model", RenderNode.model_matrix);
                RenderNode.ref_material->m_AlbedoMap->Bind(0);
                RenderNode.ref_material->m_NormalMap->Bind(1);
                RenderNode.ref_material->m_MetallicMap->Bind(2);
                RenderNode.ref_material->m_RoughnessMap->Bind(3);
                RenderNode.ref_material->m_OcclusionMap->Bind(4);
                RenderNode.ref_material->m_EmissiveMap->Bind(5);

                m_Shader->SetFloat3("u_Albedo", RenderNode.ref_material->m_Albedo);
                m_Shader->SetFloat("u_Roughness", RenderNode.ref_material->m_Roughness);
                m_Shader->SetFloat("u_Metallic", RenderNode.ref_material->m_Metallic);

                m_Shader->SetInt("u_UseAlbedoMap", RenderNode.ref_material->b_Albedo);
                m_Shader->SetInt("u_UseNormalMap", RenderNode.ref_material->b_Normal);
                m_Shader->SetInt("u_UseMetallicMap", RenderNode.ref_material->b_Metallic);
                m_Shader->SetInt("u_UseRoughnessMap", RenderNode.ref_material->b_Roughness);
                m_Shader->SetInt("u_UseOcclusionMap", RenderNode.ref_material->b_Occlusion);

                RenderCommand::DrawIndexed(RenderNode.ref_mesh->m_VAO, RenderNode.ref_mesh->m_IndexCount);
            }
        }

        m_Framebuffer->Unbind();
    }

    void MainCameraRenderPass::OnResize(uint32_t width, uint32_t height) { m_Framebuffer->Resize(width, height); }

    int MainCameraRenderPass::GetMousePicking(int x, int y) { return m_Framebuffer->ReadPixel(1, x, y); }

    void ShadowMapRenderPass::Init(uint32_t width, uint32_t height) {
        FramebufferSpecification fbSpec;
        fbSpec.Attachments = {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_DEPTH};
        fbSpec.Width = m_ShadowMapSize;
        fbSpec.Height = m_ShadowMapSize;
        m_Framebuffer = Framebuffer::Create(fbSpec);

        m_LightPos = glm::vec3(-2.0f, 4.0f, -1.0f);
        CalculateViewProjectionMatrix();

        m_Shader = m_Shader = Shader::Create(m_ShaderPath);
        m_Shader->Bind();
    }

    void ShadowMapRenderPass::Draw() {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0, 0, 0, 0});
        RenderCommand::Clear();
        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::FRONT);

        const auto& visiableRenderNode = *m_VisiableNodes.p_main_camera_visible_mesh_nodes;
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
        if (!visiableRenderNode.empty()) {
            for (auto& RenderNode : visiableRenderNode) {
                RenderNode.ref_mesh->m_VAO->Bind();
                m_Shader->SetMat4("u_Model", RenderNode.model_matrix);
                RenderCommand::DrawIndexed(RenderNode.ref_mesh->m_VAO, RenderNode.ref_mesh->m_IndexCount);
            }
        }

        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::BACK);
        m_Framebuffer->Unbind();
    }

    void ShadowMapRenderPass::OnResize(uint32_t width, uint32_t height) {
        //* framebuffer dont need to resize
        // m_Framebuffer->Resize(width, height);
    }

    void ShadowMapRenderPass::CalculateViewProjectionMatrix() {
        GLfloat near_plane = 0.1f, far_plane = 100.0f;
        glm::mat4 lightProjection = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
        // glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, near_plane, far_plane);
        glm::mat4 lightView = glm::lookAt(m_LightPos, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        m_ViewProjectionMatrix = lightProjection * lightView;
    }

    void SkyboxRenderPass::Init(uint32_t width, uint32_t height) {
        m_SkyboxShader = Shader::Create(m_SkyboxShaderPath);
        m_SkyboxShader->Bind();
        m_SkyboxShader->SetInt("u_SkyboxTexture", 0);
    }

    void SkyboxRenderPass::Draw() {
        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LEQUAL);
        m_SkyboxShader->Bind();
        m_SkyboxShader->SetMat4("u_Projection", m_SkyboxProjection);
        m_SkyboxShader->SetMat4("u_View", m_SkyboxView);

        RenderCommand::DrawIndexed(m_Cube.m_VAO, m_Cube.m_IndexCount);

        RenderCommand::SetDepthFunc(ETHEREAL_DEPTH_FUNC::LESS);
    }

    void SkyboxRenderPass::OnResize(uint32_t width, uint32_t height) {}

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