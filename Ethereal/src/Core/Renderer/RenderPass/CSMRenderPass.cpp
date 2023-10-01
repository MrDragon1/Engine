#include "CSMRenderPass.h"

#include <Base/GlobalContext.h>
#include "Utils/StringUtils.h"
namespace Ethereal {
void CSMRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    ShaderSourceString source;
    source[ShaderType::VERTEX] = Utils::LoadShader("assets/shaders/CSM/CSM.vert");
    source[ShaderType::GEOMETRY] = Utils::LoadShader("assets/shaders/CSM/CSM.geom");
    source[ShaderType::FRAGMENT] = Utils::LoadShader("assets/shaders/CSM/CSM.frag");
    mPipelineState.program = api->CreateProgram("CSM", source);

    Project::GetConfigManager().sCSMConfig.ShadowMap = api->CreateTexture(
        1, Project::GetConfigManager().sCSMConfig.ShadowMapSize,
        Project::GetConfigManager().sCSMConfig.ShadowMapSize,
        Project::GetConfigManager().sCSMConfig.CascadeCount + 1, TextureFormat::DEPTH,
        TextureUsage::DEFAULT | TextureUsage::DEPTH_ATTACHMENT, TextureType::TEXTURE_2D_ARRAY);

    TargetBufferInfo depthInfo{};
    depthInfo.handle = Project::GetConfigManager().sCSMConfig.ShadowMap;
    depthInfo.layerCount = depthInfo.handle->depth;

    mRenderTarget = api->CreateRenderTarget(
        TargetBufferFlags::DEPTH, Project::GetConfigManager().sCSMConfig.ShadowMapSize,
        Project::GetConfigManager().sCSMConfig.ShadowMapSize, {}, depthInfo, {});

    mParams.clearColor = {0, 0, 0, 0};
    mPipelineState.rasterState.depthFunc = RasterState::DepthFunc::L;
    mParams.viewport = {0, 0, Project::GetConfigManager().sCSMConfig.ShadowMapSize,
                        Project::GetConfigManager().sCSMConfig.ShadowMapSize};
    mParams.flags.clearMask = TargetBufferFlags::DEPTH;
    mPipelineState.rasterState.colorTargetCount = 0;
    mPipelineState.rasterState.rasterizationSamples = 1;
}

void CSMRenderPass::Draw() {
    ET_PROFILE_FUNC();
    UpdateDistance();
    // Draw Shadow Map
    const auto& staticMeshDrawList = mDrawLists.StaticMeshDrawList;
    const auto& meshDrawList = mDrawLists.MeshDrawList;
    const auto& meshTransformMap = mDrawLists.MeshTransformMap;
    uint32_t drawIndex = 0;
    auto count = Project::GetConfigManager().sCSMConfig.CascadeCount;
    auto& splits = Project::GetConfigManager().sCSMConfig.CascadeSplits;
    for (int i = 0; i < count; i++) {
        splits[i].x = mDistance[i];  // Refer to the comment in RenderSystem.h of ShadowData
    }
    CalculateLightSpaceMatrices();

    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();
    uniformManager->UpdateShadow(nullptr);
    api->BeginRenderPass(mRenderTarget, mParams);
    // Draw Static Mesh
    if (!staticMeshDrawList.empty()) {
        for (auto& [mk, dc] : staticMeshDrawList) {
            Ref<MeshSource> ms = dc.StaticMesh->GetMeshSource();
            Ref<MaterialTable> mt = dc.MaterialTable;
            Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];

            uniformManager->UpdateRenderPrimitive(
                {.ModelMatrix = meshTransformMap.at(mk).Transforms[0].Transform}, drawIndex);
            uniformManager->Commit();
            uniformManager->Bind(drawIndex);
            // RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount,
            // reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
            //                            submesh.BaseVertex);

            // api->Draw(ms->GetRenderPrimitive(), mPipelineState);
            api->Draw(ms->GetSubMeshRenderPrimitive(dc.SubmeshIndex), mPipelineState);

            drawIndex++;
        }
    }

    api->EndRenderPass();
}

void CSMRenderPass::OnResize(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    // api->ResizeRenderTarget(mRenderTarget, width, height);
}

std::vector<Vector4> CSMRenderPass::GetFrustumCornersWorldSpace(const Matrix4& projview) {
    const auto inv = Math::Inverse(projview);

    std::vector<Vector4> frustumCorners;
    for (unsigned int x = 0; x < 2; ++x) {
        for (unsigned int y = 0; y < 2; ++y) {
            for (unsigned int z = 0; z < 2; ++z) {
                const Vector4 pt =
                    inv * Vector4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                frustumCorners.push_back(pt / pt.w);
            }
        }
    }

    return frustumCorners;
}

std::vector<Vector4> CSMRenderPass::GetFrustumCornersWorldSpace(const Matrix4& proj,
                                                                const Matrix4& view) {
    return GetFrustumCornersWorldSpace(proj * view);
}

Matrix4 CSMRenderPass::GetLightSpaceMatrix(float nearPlane, float farPlane) {
    const auto proj =
        Math::Perspective(Math::Radians(Project::GetConfigManager().sCSMConfig.FOV),
                          Project::GetConfigManager().sCSMConfig.AspectRatio, nearPlane, farPlane);
    const auto corners =
        GetFrustumCornersWorldSpace(proj, Project::GetConfigManager().sCSMConfig.ViewMatrix);

    Vector3 center = Vector3(0, 0, 0);
    for (const auto& v : corners) {
        center += Vector3(v.x, v.y, v.z);
    }
    center /= corners.size();

    const auto lightView = Math::LookAt(
        center + Project::GetConfigManager().sUniformManagerConfig.LightParam.Direction, center,
        Vector3(0.0f, 1.0f, 0.0f));

    float minX = std::numeric_limits<float>::max();
    float maxX = std::numeric_limits<float>::min();
    float minY = std::numeric_limits<float>::max();
    float maxY = std::numeric_limits<float>::min();
    float minZ = std::numeric_limits<float>::max();
    float maxZ = std::numeric_limits<float>::min();
    for (const auto& v : corners) {
        const auto trf = lightView * v;
        minX = std::min(minX, trf.x);
        maxX = std::max(maxX, trf.x);
        minY = std::min(minY, trf.y);
        maxY = std::max(maxY, trf.y);
        minZ = std::min(minZ, trf.z);
        maxZ = std::max(maxZ, trf.z);
    }

    // Tune this parameter according to the scene
    constexpr float zMult = 10.0f;
    if (minZ < 0) {
        minZ *= zMult;
    } else {
        minZ /= zMult;
    }
    if (maxZ < 0) {
        maxZ /= zMult;
    } else {
        maxZ *= zMult;
    }

    const Matrix4 lightProjection = Math::Ortho(minX, maxX, minY, maxY, minZ, maxZ);

    return lightProjection * lightView;
}

void CSMRenderPass::CalculateLightSpaceMatrices() {
    std::vector<Matrix4> ret;
    auto count = Project::GetConfigManager().sCSMConfig.CascadeCount;
    auto& matrices = Project::GetConfigManager().sCSMConfig.DirLightMatrices;

    for (size_t i = 0; i < count + 1; ++i) {
        if (i == 0) {
            matrices[i] =
                GetLightSpaceMatrix(Project::GetConfigManager().sCSMConfig.NearPlane, mDistance[i]);
        } else if (i < mDistance.size()) {
            matrices[i] = GetLightSpaceMatrix(mDistance[i - 1], mDistance[i]);
        } else {
            matrices[i] = GetLightSpaceMatrix(mDistance[i - 1],
                                              Project::GetConfigManager().sCSMConfig.FarPlane);
        }
    }
}

void CSMRenderPass::UpdateDistance() {
    auto nearPlane = Project::GetConfigManager().sCSMConfig.NearPlane;
    auto farPlane = Project::GetConfigManager().sCSMConfig.FarPlane;
    // TODO: automatically generate distance according to mCascaded
    mDistance.clear();
    mDistance.push_back(farPlane / 50.0f);
    mDistance.push_back(farPlane / 25.0f);
    mDistance.push_back(farPlane / 10.0f);
    mDistance.push_back(farPlane / 2.0f);
}
}  // namespace Ethereal