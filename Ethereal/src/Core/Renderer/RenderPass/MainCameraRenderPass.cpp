#include "MainCameraRenderPass.h"

namespace Ethereal
{
    void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
        m_ViewProjectionMatrix = Matrix4::IDENTITY;

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

        /* Static Mesh Shader */
        m_StaticMeshShader = Shader::Create(m_StaticMeshShaderPath);
        m_StaticMeshShader->Bind();

        m_StaticMeshShader->SetFloat3("u_Albedo", {0.5f, 0.0f, 0.0f});
        m_StaticMeshShader->SetFloat("u_Occlusion", 1.0f);

        m_StaticMeshShader->SetInt("u_AlbedoMap", 0);
        m_StaticMeshShader->SetInt("u_NormalMap", 1);
        m_StaticMeshShader->SetInt("u_MetallicMap", 2);
        m_StaticMeshShader->SetInt("u_RoughnessMap", 3);
        m_StaticMeshShader->SetInt("u_OcclusionMap", 4);

        // IBL
        m_StaticMeshShader->SetInt("u_IrradianceMap", 6);
        m_StaticMeshShader->SetInt("u_PrefilterMap", 7);
        m_StaticMeshShader->SetInt("u_BRDFLUT", 8);

        m_StaticMeshShader->SetInt("u_ShadowMap", 9);

        /* Mesh Shader */
        m_MeshShader = Shader::Create(m_MeshShaderPath);
        m_MeshShader->Bind();

        m_MeshShader->SetFloat3("u_Albedo", {0.5f, 0.0f, 0.0f});
        m_MeshShader->SetFloat("u_Occlusion", 1.0f);

        m_MeshShader->SetInt("u_AlbedoMap", 0);
        m_MeshShader->SetInt("u_NormalMap", 1);
        m_MeshShader->SetInt("u_MetallicMap", 2);
        m_MeshShader->SetInt("u_RoughnessMap", 3);
        m_MeshShader->SetInt("u_OcclusionMap", 4);

        // IBL
        m_MeshShader->SetInt("u_IrradianceMap", 6);
        m_MeshShader->SetInt("u_PrefilterMap", 7);
        m_MeshShader->SetInt("u_BRDFLUT", 8);

        m_MeshShader->SetInt("u_ShadowMap", 9);
    }

    void MainCameraRenderPass::Draw() {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();
        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::BACK);
        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        const auto& staticMeshDrawList = m_DrawLists.StaticMeshDrawList;
        const auto& meshDrawList = m_DrawLists.MeshDrawList;
        const auto& meshTransformMap = m_DrawLists.MeshTransformMap;

        // lights
        Vector3 lightPositions[] = {
            Vector3(-10.0f, 20.0f, 10.0f),
            Vector3(10.0f, 20.0f, 10.0f),
            Vector3(-10.0f, 0.0f, 10.0f),
            Vector3(10.0f, 0.0f, 10.0f),
        };
        Vector3 lightColors[] = {Vector3(300.0f, 300.0f, 300.0f), Vector3(300.0f, 300.0f, 300.0f), Vector3(300.0f, 300.0f, 300.0f),
                                   Vector3(300.0f, 300.0f, 300.0f)};

        // Draw StaticMesh
        {
            m_StaticMeshShader->Bind();
            m_StaticMeshShader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
            m_StaticMeshShader->SetFloat3("camPos", m_CameraPosition);

            for (int i = 0; i < 4; i++) {
                m_StaticMeshShader->SetFloat3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
                m_StaticMeshShader->SetFloat3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
            }

            // CSM
            m_CSMData.ShadowMap->Bind(9);
            m_StaticMeshShader->SetMat4("u_View", m_CSMData.View);
            m_StaticMeshShader->SetFloat("u_FarPlane", m_CSMData.FarPlane);
            m_StaticMeshShader->SetFloat3("u_LightDir", m_CSMData.LightDir);
            m_StaticMeshShader->SetInt("u_CascadeCount", m_CSMData.Cascaded);
            for (int i = 0; i < m_CSMData.LightMatrices.size(); i++) {
                m_StaticMeshShader->SetMat4("u_LightSpaceMatrices[" + std::to_string(i) + "]", m_CSMData.LightMatrices[i]);
            }
            for (int i = 0; i < m_CSMData.Distance.size(); i++) {
                m_StaticMeshShader->SetFloat("u_CascadePlaneDistances[" + std::to_string(i) + "]", m_CSMData.Distance[i]);
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
                    m_StaticMeshShader->SetMat4("u_Model", meshTransformMap.at(mk).Transforms[0].Transform);
                    m_StaticMeshShader->SetInt("u_EntityID", mk.EntityID);

                    material->GetAlbedoMap()->Bind(0);
                    material->GetNormalMap()->Bind(1);
                    material->GetMetalnessMap()->Bind(2);
                    material->GetRoughnessMap()->Bind(3);
                    material->GetOcclusionMap()->Bind(4);

                    m_StaticMeshShader->SetFloat3("u_Albedo", material->GetAlbedoColor());
                    m_StaticMeshShader->SetFloat("u_Roughness", material->GetRoughness());
                    m_StaticMeshShader->SetFloat("u_Metallic", material->GetMetalness());
                    m_StaticMeshShader->SetFloat("u_Emisstion", material->GetEmission());

                    m_StaticMeshShader->SetInt("u_UseAlbedoMap", material->IsUseAlbedoMap());
                    m_StaticMeshShader->SetInt("u_UseNormalMap", material->IsUseNormalMap());
                    m_StaticMeshShader->SetInt("u_UseMetallicMap", material->IsUseMetallicMap());
                    m_StaticMeshShader->SetInt("u_UseRoughnessMap", material->IsUseRoughnessMap());
                    m_StaticMeshShader->SetInt("u_UseOcclusionMap", material->IsUseOcclusionMap());

                    RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
                                               reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)), submesh.BaseVertex);
                }
            }
        }

        // Draw Mesh
        {
            m_MeshShader->Bind();
            m_MeshShader->SetMat4("u_ViewProjection", m_ViewProjectionMatrix);
            m_MeshShader->SetFloat3("camPos", m_CameraPosition);

            for (int i = 0; i < 4; i++) {
                m_MeshShader->SetFloat3("lightPositions[" + std::to_string(i) + "]", lightPositions[i]);
                m_MeshShader->SetFloat3("lightColors[" + std::to_string(i) + "]", lightColors[i]);
            }

            // CSM
            m_CSMData.ShadowMap->Bind(9);
            m_MeshShader->SetMat4("u_View", m_CSMData.View);
            m_MeshShader->SetFloat("u_FarPlane", m_CSMData.FarPlane);
            m_MeshShader->SetFloat3("u_LightDir", m_CSMData.LightDir);
            m_MeshShader->SetInt("u_CascadeCount", m_CSMData.Cascaded);
            for (int i = 0; i < m_CSMData.LightMatrices.size(); i++) {
                m_MeshShader->SetMat4("u_LightSpaceMatrices[" + std::to_string(i) + "]", m_CSMData.LightMatrices[i]);
            }
            for (int i = 0; i < m_CSMData.Distance.size(); i++) {
                m_MeshShader->SetFloat("u_CascadePlaneDistances[" + std::to_string(i) + "]", m_CSMData.Distance[i]);
            }

            // Draw
            if (!meshDrawList.empty()) {
                for (auto& [mk, dc] : meshDrawList) {
                    Ref<MeshSource> ms = dc.Mesh->GetMeshSource();

                    Ref<Animator> animator = ms->GetAnimator();
                    auto boneMatrices = animator->GetFinalBoneMatrices();
                    for (const auto& [id, m] : boneMatrices) {
                        if (id >= 100) {
                            ET_CORE_WARN("Only support 100 bones!");
                            continue;
                        }
                        m_MeshShader->SetMat4("u_BoneTransforms[" + std::to_string(id) + "]", m);
                    }

                    Ref<MaterialTable> mt = dc.MaterialTable;
                    const auto& meshMaterialTable = dc.Mesh->GetMaterials();
                    Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];
                    auto materialIndex = submesh.MaterialIndex;

                    Ref<MaterialAsset> material =
                        mt->HasMaterial(materialIndex) ? mt->GetMaterial(materialIndex) : meshMaterialTable->GetMaterial(materialIndex);

                    ms->GetVertexArray()->Bind();
                    m_MeshShader->SetMat4("u_Model", meshTransformMap.at(mk).Transforms[0].Transform);
                    m_MeshShader->SetInt("u_EntityID", mk.EntityID);

                    material->GetAlbedoMap()->Bind(0);
                    material->GetNormalMap()->Bind(1);
                    material->GetMetalnessMap()->Bind(2);
                    material->GetRoughnessMap()->Bind(3);
                    material->GetOcclusionMap()->Bind(4);

                    m_MeshShader->SetFloat3("u_Albedo", material->GetAlbedoColor());
                    m_MeshShader->SetFloat("u_Roughness", material->GetRoughness());
                    m_MeshShader->SetFloat("u_Metallic", material->GetMetalness());
                    m_MeshShader->SetFloat("u_Emisstion", material->GetEmission());

                    m_MeshShader->SetInt("u_UseAlbedoMap", material->IsUseAlbedoMap());
                    m_MeshShader->SetInt("u_UseNormalMap", material->IsUseNormalMap());
                    m_MeshShader->SetInt("u_UseMetallicMap", material->IsUseMetallicMap());
                    m_MeshShader->SetInt("u_UseRoughnessMap", material->IsUseRoughnessMap());
                    m_MeshShader->SetInt("u_UseOcclusionMap", material->IsUseOcclusionMap());

                    RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
                                               reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)), submesh.BaseVertex);
                }
            }
        }

        m_Framebuffer->Unbind();
    }

    void MainCameraRenderPass::OnResize(uint32_t width, uint32_t height) { m_Framebuffer->Resize(width, height); }

    int MainCameraRenderPass::GetMousePicking(int x, int y) { return m_Framebuffer->ReadPixel(1, x, y); }
}  // namespace Ethereal
