#include "MainCameraRenderPass.h"

#include <Base/GlobalContext.h>

namespace Ethereal {
void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    ShaderSource source;
    source[ShaderType::VERTEX] = PBR_VERT;
    source[ShaderType::FRAGMENT] = PBR_FRAG;
    mStaticMeshPipeline.program = api->CreateProgram("PBR", source);

    source[ShaderType::VERTEX] = PBRANIM_VERT;
    source[ShaderType::FRAGMENT] = PBRANIM_FRAG;
    mMeshPipeline.program = api->CreateProgram("PBRANIM", source);

    source[ShaderType::VERTEX] = SKYBOX_VERT;
    source[ShaderType::FRAGMENT] = SKYBOX_FRAG;
    mSkyboxPipeline.program = api->CreateProgram("SKYBOX", source);
    mSkyboxPipeline.rasterState.depthFunc = RasterState::DepthFunc::LE;

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                                     TextureType::TEXTURE_2D);
    auto entityIdTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32_INTEGER, usage,
                                          TextureType::TEXTURE_2D);
    auto depthTex = api->CreateTexture(1, width, height, 1, TextureFormat::DEPTH,
                                       TextureUsage::DEPTH_ATTACHMENT, TextureType::TEXTURE_2D);
    MRT mrt;
    mrt[0] = {hdrTex};
    mrt[1] = {entityIdTex};
    mRenderTarget = api->CreateRenderTarget(
        TargetBufferFlags::COLOR0 | TargetBufferFlags::COLOR1 | TargetBufferFlags::DEPTH, width,
        height, mrt, {depthTex}, {});
}

void MainCameraRenderPass::Draw() {
    mParams.clearColor = {0.1, 0.1, 0.1, 1.0};
    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();

    /*************************************************************************/

    // Clear our entity ID attachment to -1

    const auto& staticMeshDrawList = mDrawLists.StaticMeshDrawList;
    const auto& meshDrawList = mDrawLists.MeshDrawList;
    const auto& meshTransformMap = mDrawLists.MeshTransformMap;

    api->BeginRenderPass(mRenderTarget, mParams);

    // Draw StaticMesh
    {
        // CSM

        uniformManager->UpdateShadow(Project::GetConfigManager().sCSMConfig.ShadowMap);

        // Draw Static Mesh
        if (!staticMeshDrawList.empty()) {
            for (auto& [mk, dc] : staticMeshDrawList) {
                Ref<MeshSource> ms = dc.StaticMesh->GetMeshSource();
                Ref<MaterialTable> mt = dc.MaterialTable;
                const auto& meshMaterialTable = dc.StaticMesh->GetMaterials();
                Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];
                auto materialIndex = submesh.MaterialIndex;

                Ref<MaterialAsset> material = mt->HasMaterial(materialIndex)
                                                  ? mt->GetMaterial(materialIndex)
                                                  : meshMaterialTable->GetMaterial(materialIndex);

                Project::GetConfigManager().sUniformManagerConfig.EditorParam.EntityID =
                    mk.EntityID;

                uniformManager->UpdateMaterial(material);
                uniformManager->UpdateEditor();
                uniformManager->UpdateRenderPrimitive(
                    {.ModelMatrix = meshTransformMap.at(mk).Transforms[0].Transform});
                // TODO: should not update ViewUib here ( UpdateEditor() will cause this problem )
                uniformManager->Commit();
                uniformManager->Bind();

                // RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
                // reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
                //                            submesh.BaseVertex);

                api->Draw(ms->GetSubMeshRenderPrimitive(dc.SubmeshIndex), mStaticMeshPipeline);
            }
        }
    }

    // Draw Mesh
    {
        // CSM
        uniformManager->UpdateShadow(Project::GetConfigManager().sCSMConfig.ShadowMap);

        // Draw
        if (!meshDrawList.empty()) {
            for (auto& [mk, dc] : meshDrawList) {
                Ref<MeshSource> ms = dc.Mesh->GetMeshSource();

                Ref<Animator> animator = ms->GetAnimator();
                auto boneMatrices = animator->GetFinalBoneMatrices();
                BoneParam& param = Project::GetConfigManager().sUniformManagerConfig.BoneParam;
                for (const auto& [id, m] : boneMatrices) {
                    if (id >= 100) {
                        ET_CORE_WARN("Only support 100 bones!");
                        continue;
                    }
                    param.BoneTransform[id] = m;
                }

                Ref<MaterialTable> mt = dc.MaterialTable;
                const auto& meshMaterialTable = dc.Mesh->GetMaterials();
                Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];
                auto materialIndex = submesh.MaterialIndex;

                Ref<MaterialAsset> material = mt->HasMaterial(materialIndex)
                                                  ? mt->GetMaterial(materialIndex)
                                                  : meshMaterialTable->GetMaterial(materialIndex);

                Project::GetConfigManager().sUniformManagerConfig.EditorParam.EntityID =
                    mk.EntityID;

                uniformManager->UpdateBone();
                uniformManager->UpdateMaterial(material);
                uniformManager->UpdateEditor();
                uniformManager->UpdateRenderPrimitive(
                    {.ModelMatrix = meshTransformMap.at(mk).Transforms[0].Transform});
                // TODO: should not update ViewUib here ( UpdateEditor() will cause this problem )
                uniformManager->Commit();
                uniformManager->Bind();

                api->Draw(ms->GetSubMeshRenderPrimitive(dc.SubmeshIndex), mMeshPipeline);

                // RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
                // reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
                //                            submesh.BaseVertex);
            }
        }
    }

    // Draw Skybox
    uniformManager->Bind();
    api->Draw(RenderResource::Cube->GetMeshSource()->GetRenderPrimitive(), mSkyboxPipeline);

    api->EndRenderPass();
}

void MainCameraRenderPass::OnResize(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    // TODO: move this stuff to api
    api->DestroyRenderTarget(mRenderTarget);

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                                     TextureType::TEXTURE_2D);
    auto entityIdTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32_INTEGER, usage,
                                          TextureType::TEXTURE_2D);
    auto depthTex = api->CreateTexture(1, width, height, 1, TextureFormat::DEPTH,
                                       TextureUsage::DEPTH_ATTACHMENT, TextureType::TEXTURE_2D);
    MRT mrt;
    mrt[0] = {hdrTex};
    mrt[1] = {entityIdTex};
    mRenderTarget = api->CreateRenderTarget(
        TargetBufferFlags::COLOR0 | TargetBufferFlags::COLOR1 | TargetBufferFlags::DEPTH, width,
        height, mrt, {depthTex}, {});
}

int MainCameraRenderPass::GetMousePicking(int x, int y) {
    auto api = GlobalContext::GetDriverApi();
    return api->ReadPixel(mRenderTarget, 1, x, y);
}
}  // namespace Ethereal
