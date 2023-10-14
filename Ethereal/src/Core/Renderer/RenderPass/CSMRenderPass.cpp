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
            // uniformManager->Commit();
            uniformManager->Bind(drawIndex);
            
            api->Draw(ms->GetSubMeshRenderPrimitive(dc.SubmeshIndex), mPipelineState);

            drawIndex++;
        }
    }
    // Draw Mesh
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
            uniformManager->UpdateBone(drawIndex);
            uniformManager->UpdateRenderPrimitive(
                {.ModelMatrix = meshTransformMap.at(mk).Transforms[0].Transform}, drawIndex);
            // uniformManager->Commit();
            uniformManager->Bind(drawIndex);

            api->Draw(ms->GetSubMeshRenderPrimitive(dc.SubmeshIndex), mPipelineState);
            drawIndex++;
        }
    }
    
    uniformManager->CommitBuffer();
    api->EndRenderPass();
}

void CSMRenderPass::OnResize(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    // api->ResizeRenderTarget(mRenderTarget, width, height);
}

std::vector<Vector4> CSMRenderPass::GetFrustumCornersWorldSpace(const Matrix4& projview) {
    const auto inv = Math::Inverse(projview);

    std::vector<Vector4> frustumCorners;
    for (unsigned int z = 0; z < 2; ++z) {
        for (unsigned int y = 0; y < 2; ++y) {
            for (unsigned int x = 0; x < 2; ++x) {
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

    //// refer to https://zhuanlan.zhihu.com/p/515385379 for more details
    //// calculate the circumscribed circle of the frustum
    //float a2 = pow(Math::Length(corners[3] - corners[0]),2);
    //float b2 = pow(Math::Length(corners[7] - corners[4]),2);
    //float len = farPlane - nearPlane;
    //float x = len * 0.5f + (a2 - b2) / (8.0f * len); 

    //float zCascadeDistance = len - x;

    //Vector3 sphereCenterVS = Vector3(0.0f, 0.0f, nearPlane + zCascadeDistance);
    //Vector3 sphereCenterWS =
    //    Project::GetConfigManager().sUniformManagerConfig.MainCameraParam.CameraPosition +
    //    Project::GetConfigManager().sUniformManagerConfig.MainCameraParam.ViewDirection *
    //        sphereCenterVS.z;

    //float sphereRadius = sqrtf(zCascadeDistance * zCascadeDistance + (a2 * 0.25f));

    //Vector3 sceneCenter = Vector3::ZERO; // TODO: calculate the scene center
    //float backDistance =  sphereRadius;

    //const auto lightView = Math::LookAt(
    //    sphereCenterWS +
    //        Project::GetConfigManager().sUniformManagerConfig.LightParam.GetDirection() *
    //            backDistance,
    //    sphereCenterWS, Vector3(0.0f, 1.0f, 0.0f));


    Vector3 center = Vector3(0, 0, 0);
    for (const auto& v : corners) {
        center += Vector3(v.x, v.y, v.z);
    }
    center /= corners.size();

    float radius = 0.0f;
    for (uint32_t i = 0; i < 8; i++) {
        float distance = Math::Length(corners[i] - center);
        radius = radius > distance ? radius : distance;
    }
    radius = std::ceil(radius * 16.0f) / 16.0f;

    Vector3 maxExtents = Vector3(radius);
    Vector3 minExtents = -maxExtents;

    const auto lightView = Math::LookAt(
        center +
            Project::GetConfigManager().sUniformManagerConfig.LightParam.GetDirection() *
                     maxExtents.z,
        center, Vector3(0.0f, 1.0f, 0.0f));

    // const Matrix4 lightProjection = Math::Ortho(-sphereRadius, sphereRadius, -sphereRadius, sphereRadius, 0.0f, backDistance * 2.0f);
    const Matrix4 lightProjection = Math::Ortho(minExtents.x, maxExtents.x, minExtents.y,
                                                maxExtents.y, 0.0f, maxExtents.z - minExtents.z);

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