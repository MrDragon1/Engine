#include "MainCameraRenderPass.h"

#include <Base/GlobalContext.h>
#include "Utils/StringUtils.h"
namespace Ethereal {
void MainCameraRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    ShaderSourceString source;
    source[ShaderType::VERTEX] = Utils::ReadFileAndSkipBOM("assets/shaders/PBR.vert");
    source[ShaderType::FRAGMENT] = Utils::ReadFileAndSkipBOM("assets/shaders/PBR.frag");
    mStaticMeshPipeline.program = api->CreateProgram("PBR", source);

    source[ShaderType::VERTEX] = Utils::ReadFileAndSkipBOM("assets/shaders/PBRAnim.vert");
    source[ShaderType::FRAGMENT] = Utils::ReadFileAndSkipBOM("assets/shaders/PBRAnim.frag");
    // mMeshPipeline.program = api->CreateProgram("PBRANIM", source);

    source[ShaderType::VERTEX] = Utils::ReadFileAndSkipBOM("assets/shaders/Skybox.vert");
    source[ShaderType::FRAGMENT] = Utils::ReadFileAndSkipBOM("assets/shaders/Skybox.frag");
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
    mParams.viewport = {0, 0, width, height};
    mParams.flags.clearMask =
        TargetBufferFlags::COLOR0 | TargetBufferFlags::COLOR1 | TargetBufferFlags::DEPTH;

    mStaticMeshPipeline.rasterState.colorTargetCount = 2;
    mStaticMeshPipeline.rasterState.rasterizationSamples = 1;

    mMeshPipeline.rasterState.colorTargetCount = 2;
    mMeshPipeline.rasterState.rasterizationSamples = 1;

    mSkyboxPipeline.rasterState.colorTargetCount = 2;
    mSkyboxPipeline.rasterState.rasterizationSamples = 1;
}

void MainCameraRenderPass::Draw() {
    mParams.clearColor = {0.1, 0.1, 0.1, 1.0};
    mParams.clearDepth = 1.0f;
    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();
    uint32_t drawIndex = 0;
    /*************************************************************************/

    // Clear our entity ID attachment to -1

    const auto& staticMeshDrawList = mDrawLists.StaticMeshDrawList;
    const auto& meshDrawList = mDrawLists.MeshDrawList;
    const auto& meshTransformMap = mDrawLists.MeshTransformMap;

    api->BeginRenderPass(mRenderTarget, mParams);

    // Draw StaticMesh
    {
        // CSM

        // uniformManager->UpdateShadow(Project::GetConfigManager().sCSMConfig.ShadowMap);

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

                uniformManager->UpdateMaterial(material, drawIndex);
                uniformManager->UpdateEditor();
                uniformManager->UpdateRenderPrimitive(
                    {.ModelMatrix = meshTransformMap.at(mk).Transforms[0].Transform}, drawIndex);
                // TODO: should not update ViewUib here ( UpdateEditor() will cause this problem )
                uniformManager->Commit();
                uniformManager->Bind(drawIndex);

                // RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
                // reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
                //                            submesh.BaseVertex);

                api->Draw(ms->GetSubMeshRenderPrimitive(dc.SubmeshIndex), mStaticMeshPipeline);
                drawIndex++;
            }
        }
    }

    //// Draw Mesh
    //{
    //    // CSM
    //    uniformManager->UpdateShadow(Project::GetConfigManager().sCSMConfig.ShadowMap);

    //    // Draw
    //    if (!meshDrawList.empty()) {
    //        for (auto& [mk, dc] : meshDrawList) {
    //            Ref<MeshSource> ms = dc.Mesh->GetMeshSource();

    //            Ref<Animator> animator = ms->GetAnimator();
    //            auto boneMatrices = animator->GetFinalBoneMatrices();
    //            BoneParam& param = Project::GetConfigManager().sUniformManagerConfig.BoneParam;
    //            for (const auto& [id, m] : boneMatrices) {
    //                if (id >= 100) {
    //                    ET_CORE_WARN("Only support 100 bones!");
    //                    continue;
    //                }
    //                param.BoneTransform[id] = m;
    //            }

    //            Ref<MaterialTable> mt = dc.MaterialTable;
    //            const auto& meshMaterialTable = dc.Mesh->GetMaterials();
    //            Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];
    //            auto materialIndex = submesh.MaterialIndex;

    //            Ref<MaterialAsset> material = mt->HasMaterial(materialIndex)
    //                                              ? mt->GetMaterial(materialIndex)
    //                                              : meshMaterialTable->GetMaterial(materialIndex);

    //            Project::GetConfigManager().sUniformManagerConfig.EditorParam.EntityID =
    //                mk.EntityID;

    //            uniformManager->UpdateBone();
    //            uniformManager->UpdateMaterial(material);
    //            uniformManager->UpdateEditor();
    //            uniformManager->UpdateRenderPrimitive(
    //                {.ModelMatrix = meshTransformMap.at(mk).Transforms[0].Transform});
    //            // TODO: should not update ViewUib here ( UpdateEditor() will cause this problem )
    //            uniformManager->Commit();
    //            uniformManager->Bind();

    //            api->Draw(ms->GetSubMeshRenderPrimitive(dc.SubmeshIndex), mMeshPipeline);

    //            // RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
    //            // reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
    //            //                            submesh.BaseVertex);
    //        }
    //    }
    //}

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
    mParams.viewport = {0, 0, width, height};
}

int MainCameraRenderPass::GetMousePicking(int x, int y) {
    auto api = GlobalContext::GetDriverApi();
    return api->ReadPixel(mRenderTarget, 1, x, y);
}
}  // namespace Ethereal
