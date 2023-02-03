#include <Base/GlobalContext.h>
#include "MainCameraRenderPass.h"

namespace Ethereal
{
    void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
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
        m_StaticMeshShader = GlobalContext::GetShaderLibrary().Get("PBR");
        m_StaticMeshShader->Bind();

        /* Mesh Shader */
        m_MeshShader = GlobalContext::GetShaderLibrary().Get("PBRANIM");
        m_MeshShader->Bind();

    }

    void MainCameraRenderPass::Draw() {
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
        RenderCommand::Clear();

        // Clear our entity ID attachment to -1
        m_Framebuffer->ClearAttachment(1, -1);

        const auto& staticMeshDrawList = m_DrawLists.StaticMeshDrawList;
        const auto& meshDrawList = m_DrawLists.MeshDrawList;
        const auto& meshTransformMap = m_DrawLists.MeshTransformMap;

        auto& shaderCommonData = GlobalContext::GetRenderSystem().GetShaderCommonData();
        // Draw StaticMesh
        {
            m_StaticMeshShader->Bind();

            // CSM
            m_CSMData.ShadowMap->Bind(18);
            shaderCommonData.SceneData.DirectionalLight.Direction = m_CSMData.LightDir;

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
                    shaderCommonData.RendererData.EntityID = mk.EntityID;

                    material->GetAlbedoMap()->Bind(10);
                    material->GetNormalMap()->Bind(11);
                    material->GetMetalnessMap()->Bind(12);
                    material->GetRoughnessMap()->Bind(13);
                    material->GetOcclusionMap()->Bind(14);

                    shaderCommonData.MaterialData.u_Albedo = {material->GetAlbedoColor(), 1.0f}; // fourth element is not used
                    shaderCommonData.MaterialData.u_Roughness = material->GetRoughness();
                    shaderCommonData.MaterialData.u_Metallic = material->GetMetalness();
                    shaderCommonData.MaterialData.u_Emisstion = material->GetEmission();

                    shaderCommonData.MaterialData.u_UseMap = 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseAlbedoMap() ? 1 << 1 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseNormalMap() ? 1 << 2 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseMetallicMap() ? 1 << 3 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseRoughnessMap() ? 1 << 4 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseOcclusionMap() ? 1 << 5 : 0;

                    //TODO:
                    GlobalContext::GetRenderSystem().UpdateUniformData();

                    RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
                                               reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)), submesh.BaseVertex);
                }
            }
        }

        // Draw Mesh
        {
            m_MeshShader->Bind();

            // CSM
            m_CSMData.ShadowMap->Bind(9);
            shaderCommonData.SceneData.DirectionalLight.Direction = m_CSMData.LightDir;

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
                    shaderCommonData.RendererData.EntityID = mk.EntityID;

                    material->GetAlbedoMap()->Bind(10);
                    material->GetNormalMap()->Bind(11);
                    material->GetMetalnessMap()->Bind(12);
                    material->GetRoughnessMap()->Bind(13);
                    material->GetOcclusionMap()->Bind(14);

                    shaderCommonData.MaterialData.u_Albedo = {material->GetAlbedoColor(),0.0f}; // fourth element is not used
                    shaderCommonData.MaterialData.u_Roughness = material->GetRoughness();
                    shaderCommonData.MaterialData.u_Metallic = material->GetMetalness();
                    shaderCommonData.MaterialData.u_Emisstion = material->GetEmission();

                    shaderCommonData.MaterialData.u_UseMap = 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseAlbedoMap() ? 1 << 1 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseNormalMap() ? 1 << 2 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseMetallicMap() ? 1 << 3 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseRoughnessMap() ? 1 << 4 : 0;
                    shaderCommonData.MaterialData.u_UseMap |= material->IsUseOcclusionMap() ? 1 << 5 : 0;

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
