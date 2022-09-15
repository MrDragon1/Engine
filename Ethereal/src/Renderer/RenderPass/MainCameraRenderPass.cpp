#include "MainCameraRenderPass.h"

namespace Ethereal
{
    void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
        m_ViewProjectionMatrix = glm::mat4(1.0f);

        TextureSpecification hdrSpec, r32Spec, depthSpec;
        hdrSpec.Format = ETHEREAL_PIXEL_FORMAT::R16G16B16A16_HDR;
        r32Spec.Format = ETHEREAL_PIXEL_FORMAT::R32_INTEGER;
        depthSpec.Format = ETHEREAL_PIXEL_FORMAT::DEPTH;

        FramebufferSpecification fbSpec;
        fbSpec.ColorAttachments.PushAttachmentSpec(hdrSpec);
        fbSpec.ColorAttachments.PushAttachmentSpec(r32Spec);
        fbSpec.DepthAttachment.SetAttachmentSpec(depthSpec);
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

        // IBL 
        m_Shader->SetInt("u_IrradianceMap", 6);
        m_Shader->SetInt("u_PrefilterMap", 7);
        m_Shader->SetInt("u_BRDFLUT", 8);
    }

    void MainCameraRenderPass::Draw() {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();
        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::BACK);
        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        const auto& staticMeshDrawList = m_DrawLists.StaticMeshDrawList;
        const auto& meshTransformMap = m_DrawLists.MeshTransformMap;

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

        // Draw Static Mesh
        if (!staticMeshDrawList.empty()) {
            for (auto& [mk, dc] : staticMeshDrawList) {
                Ref<MeshSource> ms = dc.StaticMesh->GetMeshSource();
                Ref<MaterialTable> mt = dc.MaterialTable;
                const auto& meshMaterialTable = dc.StaticMesh->GetMaterials();
                Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];
                auto materialIndex = submesh.MaterialIndex;

                Ref<MaterialAsset> material =
                    mt->HasMaterial(materialIndex) ? mt->GetMaterial(materialIndex) : meshMaterialTable->GetMaterial(materialIndex);

                ms->GetVertexArray()->Bind();
                m_Shader->SetMat4("u_Model", meshTransformMap.at(mk).Transforms[0].Transform);
                m_Shader->SetInt("u_EntityID", mk.EntityID);

                material->GetAlbedoMap()->Bind(0);
                material->GetNormalMap()->Bind(1);
                material->GetMetalnessMap()->Bind(2);
                material->GetRoughnessMap()->Bind(3);
                material->GetOcclusionMap()->Bind(4);

                m_Shader->SetFloat3("u_Albedo", material->GetAlbedoColor());
                m_Shader->SetFloat("u_Roughness", material->GetRoughness());
                m_Shader->SetFloat("u_Metallic", material->GetMetalness());
                m_Shader->SetFloat("u_Emisstion", material->GetEmission());

                m_Shader->SetInt("u_UseAlbedoMap", material->IsUseAlbedoMap());
                m_Shader->SetInt("u_UseNormalMap", material->IsUseNormalMap());
                m_Shader->SetInt("u_UseMetallicMap", material->IsUseMetallicMap());
                m_Shader->SetInt("u_UseRoughnessMap", material->IsUseRoughnessMap());
                m_Shader->SetInt("u_UseOcclusionMap", material->IsUseOcclusionMap());

                RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount, reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
                                           submesh.BaseVertex);
            }
        }

        m_Framebuffer->Unbind();
    }

    void MainCameraRenderPass::OnResize(uint32_t width, uint32_t height) { m_Framebuffer->Resize(width, height); }

    int MainCameraRenderPass::GetMousePicking(int x, int y) { return m_Framebuffer->ReadPixel(1, x, y); }
}  // namespace Ethereal
