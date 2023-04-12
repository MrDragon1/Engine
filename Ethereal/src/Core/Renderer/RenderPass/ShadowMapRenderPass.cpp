#include "ShadowMapRenderPass.h"

#include <Base/GlobalContext.h>

namespace Ethereal {
void ShadowMapRenderPass::Init(uint32_t width, uint32_t height) {
    m_LightPos = Vector3(-2.0f, 4.0f, -1.0f);
    CalculateViewProjectionMatrix();

    auto api = GlobalContext::GetDriverApi();
    ShaderSource source;
    source[ShaderType::VERTEX] = SHADOWMAP_VERT;
    source[ShaderType::FRAGMENT] = SHADOWMAP_FRAG;
    mPipelineState.program = api->CreateProgram("SHADOWMAP", source);

    auto depthTex = api->CreateTexture(1, Project::GetConfigManager().sCSMConfig.ShadowMapSize, Project::GetConfigManager().sCSMConfig.ShadowMapSize,
                                       Project::GetConfigManager().sCSMConfig.CascadeCount + 1, TextureFormat::DEPTH, TextureUsage::DEPTH_ATTACHMENT,
                                       TextureType::TEXTURE_2D_ARRAY);
    mRenderTarget = api->CreateRenderTarget(TargetBufferFlags::DEPTH, Project::GetConfigManager().sCSMConfig.ShadowMapSize,
                                            Project::GetConfigManager().sCSMConfig.ShadowMapSize, {}, {depthTex}, {});
    mParams.clearColor = {0, 0, 0, 0};
}

void ShadowMapRenderPass::Draw() {
    // This pass is not complete,

    // Draw Shadow Map
    const auto& staticMeshDrawList = m_DrawLists.StaticMeshDrawList;
    const auto& meshTransformMap = m_DrawLists.MeshTransformMap;

    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();

    api->BeginRenderPass(mRenderTarget, mParams);
    if (!staticMeshDrawList.empty()) {
        for (auto& [mk, dc] : staticMeshDrawList) {
            Ref<MeshSource> ms = dc.StaticMesh->GetMeshSource();
            Ref<MaterialTable> mt = dc.MaterialTable;
            Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];

            uniformManager->UpdateRenderPrimitive({.ModelMatrix = meshTransformMap.at(mk).Transforms[0].Transform});

            // RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount, reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
            //                            submesh.BaseVertex);
            api->Draw(ms->GetRenderPrimitive(), mPipelineState);
        }
    }

    api->EndRenderPass();
}

void ShadowMapRenderPass::OnResize(uint32_t width, uint32_t height) {
    //* framebuffer dont need to resize
    // m_Framebuffer->Resize(width, height);
}

void ShadowMapRenderPass::CalculateViewProjectionMatrix() {
    GLfloat near_plane = 0.1f, far_plane = 100.0f;
    Matrix4 lightProjection = Math::Ortho(-50.0f, 50.0f, -50.0f, 50.0f, near_plane, far_plane);
    Matrix4 lightView = Math::LookAt(m_LightPos, Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f));
    m_ViewProjectionMatrix = lightProjection * lightView;
}

}  // namespace Ethereal