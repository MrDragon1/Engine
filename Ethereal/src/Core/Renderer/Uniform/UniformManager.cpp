#include "UniformManager.h"

#include "Base/GlobalContext.h"
#include "Core/Renderer/RenderResource.h"
namespace Ethereal {
void UniformManager::Init() {
    mViewUB = mApi->CreateBufferObject(mViewUib.GetSize(), BufferObjectBinding::UNIFORM,
                                       BufferUsage::STATIC);
    mApi->UpdateBufferObject(mViewUB, mViewUib.ToBufferDescriptor(), 0);

    mShadowUB = mApi->CreateBufferObject(mShadowUib.GetSize(), BufferObjectBinding::UNIFORM,
                                         BufferUsage::STATIC);
    mApi->UpdateBufferObject(mShadowUB, mShadowUib.ToBufferDescriptor(), 0);

    mLightUB = mApi->CreateBufferObject(mLightUib.GetSize(), BufferObjectBinding::UNIFORM,
                                        BufferUsage::STATIC);
    mApi->UpdateBufferObject(mLightUB, mLightUib.ToBufferDescriptor(), 0);

    mRenderPrimitiveUB = mApi->CreateBufferObject(
        mRenderPrimitiveUib.GetSize(), BufferObjectBinding::UNIFORM, BufferUsage::STATIC);
    mApi->UpdateBufferObject(mRenderPrimitiveUB, mRenderPrimitiveUib.ToBufferDescriptor(), 0);

    mRenderPrimitiveBoneUB = mApi->CreateBufferObject(
        mRenderPrimitiveBoneUib.GetSize(), BufferObjectBinding::UNIFORM, BufferUsage::STATIC);
    mApi->UpdateBufferObject(mRenderPrimitiveBoneUB, mRenderPrimitiveBoneUib.ToBufferDescriptor(),
                             0);

    // TODO: should be max supported texture amount
    mSamplerGroup = mApi->CreateSamplerGroup(20);
    mSamplerGroupDesc = SamplerGroupDescriptor(20);
}

void UniformManager::UpdateFog() {
    auto param = Project::GetConfigManager().sFogConfig;
    auto& s = mViewUib.Edit();
    s.FogColor = param.FogColor;
    s.FogStart = param.FogStart;
    s.FogDensity = param.FogDensity;
    s.FogHeight = param.FogHeight;
    s.FogHeightFalloff = param.FogHeightFalloff;
    s.FogScatteringStart = param.FogScatteringStart;
    s.FogScatteringSize = param.FogScatteringSize;
    s.FogEnable = param.FogEnable;
    s.FogFromIBL = param.FogFromIBL;
}

void UniformManager::UpdateCamera() {
    auto& param = Project::GetConfigManager().sUniformManagerConfig.MainCameraParam;
    auto& s = mViewUib.Edit();
    s.ViewProjectionMatrix = param.ViewProjectionMatrix;
    s.InverseViewProjectionMatrix = param.InverseViewProjectionMatrix;
    s.ProjectionMatrix = param.ProjectionMatrix;
    s.InverseProjectionMatrix = param.InverseProjectionMatrix;
    s.ViewMatrix = param.ViewMatrix;
    s.InverseViewMatrix = param.InverseViewMatrix;
    s.FarPlane = param.FarPlane;
    s.NearPlane = param.NearPlane;
}

void UniformManager::UpdateShadow(TextureHandle ShadowMap) {
    auto& param = Project::GetConfigManager().sCSMConfig;
    auto& s = mShadowUib.Edit();
    s.CascadeCount = param.CascadeCount;
    for (int i = 0; i < 16; i++) {
        s.CascadeSplits[i] = param.CascadeSplits[i];
        s.DirLightMatrices[i] = param.DirLightMatrices[i];
    }

    mSamplerGroupDesc[18] = SamplerDescriptor{
        .texture = ShadowMap,
        .params = SamplerParams::Default(),
        .binding = 18,
    };
}

void UniformManager::UpdateEditor() {
    auto& param = Project::GetConfigManager().sUniformManagerConfig.EditorParam;
    auto& s = mViewUib.Edit();
    s.EntityID = param.EntityID;
}

void UniformManager::UpdateScene() {
    auto& param = Project::GetConfigManager().sUniformManagerConfig.SceneParam;
    auto& s = mViewUib.Edit();
    s.CameraPosition = param.CameraPosition;
    s.EnvironmentMapIntensity = param.EnvironmentMapIntensity;
    s.ScissorNormalized = param.ScissorNormalized;

    mSamplerGroupDesc[17] = SamplerDescriptor{
        .texture = param.Environment->RadianceMap,
        .params = SamplerParams::Default(),
        .binding = 17,
    };

    mSamplerGroupDesc[16] = SamplerDescriptor{
        .texture = param.Environment->IrradianceMap,
        .params = SamplerParams::Default(),
        .binding = 16,
    };

    mSamplerGroupDesc[15] = SamplerDescriptor{
        .texture = RenderResource::BRDFLutTexture,
        .params = SamplerParams::Default(),
        .binding = 15,
    };
}

void UniformManager::UpdateLight() {
    auto& param = Project::GetConfigManager().sUniformManagerConfig.LightParam;
    auto& s = mLightUib.Edit();
    s.Direction = param.Direction;
    s.Multiplier = param.Multiplier;
    s.Radiance = param.Radiance;
    s.ShadowAmount = param.ShadowAmount;
}

void UniformManager::UpdateMaterial(Ref<MaterialAsset> mat) {
    auto& s = mRenderPrimitiveUib.Edit();
    s.Albedo = Vector4(mat->GetAlbedoColor(), 1);
    s.Emisstion = mat->GetEmission();
    s.Metallic = mat->GetMetalness();
    s.Occlusion = 0;
    s.Roughness = mat->GetRoughness();

    s.UseMap = 0;
    s.UseMap |= mat->IsUseAlbedoMap() ? 1 << 1 : 0;
    s.UseMap |= mat->IsUseNormalMap() ? 1 << 2 : 0;
    s.UseMap |= mat->IsUseMetallicMap() ? 1 << 3 : 0;
    s.UseMap |= mat->IsUseRoughnessMap() ? 1 << 4 : 0;

    mSamplerGroupDesc[10] = SamplerDescriptor{
        .texture = mat->GetAlbedoMap(),
        .params = SamplerParams::Default(),
        .binding = 10,
    };
    mSamplerGroupDesc[11] = SamplerDescriptor{
        .texture = mat->GetNormalMap(),
        .params = SamplerParams::Default(),
        .binding = 11,
    };
    mSamplerGroupDesc[12] = SamplerDescriptor{
        .texture = mat->GetMetalnessMap(),
        .params = SamplerParams::Default(),
        .binding = 12,
    };
    mSamplerGroupDesc[13] = SamplerDescriptor{
        .texture = mat->GetRoughnessMap(),
        .params = SamplerParams::Default(),
        .binding = 13,
    };
    mSamplerGroupDesc[14] = SamplerDescriptor{
        .texture = mat->GetOcclusionMap(),
        .params = SamplerParams::Default(),
        .binding = 14,
    };
}

void UniformManager::UpdateRenderPrimitive(RenderPrimitiveParam param) {
    auto& s = mRenderPrimitiveUib.Edit();
    s.ModelMatrix = param.ModelMatrix;
}

void UniformManager::UpdateBone() {
    auto& param = Project::GetConfigManager().sUniformManagerConfig.BoneParam;
    auto& s = mRenderPrimitiveBoneUib.Edit();
    for (int i = 0; i < 100; i++) {
        s.BoneTransform[i] = param.BoneTransform[i];
    }
}

void UniformManager::Commit() {
    if (mViewUib.IsDirty()) mApi->UpdateBufferObject(mViewUB, mViewUib.ToBufferDescriptor(), 0);
    if (mShadowUib.IsDirty())
        mApi->UpdateBufferObject(mShadowUB, mShadowUib.ToBufferDescriptor(), 0);
    if (mLightUib.IsDirty()) mApi->UpdateBufferObject(mLightUB, mLightUib.ToBufferDescriptor(), 0);
    if (mRenderPrimitiveUib.IsDirty())
        mApi->UpdateBufferObject(mRenderPrimitiveUB, mRenderPrimitiveUib.ToBufferDescriptor(), 0);
    if (mRenderPrimitiveBoneUib.IsDirty())
        mApi->UpdateBufferObject(mRenderPrimitiveBoneUB,
                                 mRenderPrimitiveBoneUib.ToBufferDescriptor(), 0);

    mApi->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
}

void UniformManager::Bind() {
    // TODO:manage binding points
    mApi->BindUniformBuffer(0, mViewUB);
    mApi->BindUniformBuffer(1, mShadowUB);
    mApi->BindUniformBuffer(2, mLightUB);
    mApi->BindUniformBuffer(3, mRenderPrimitiveUB);
    mApi->BindUniformBuffer(4, mRenderPrimitiveBoneUB);

    mApi->BindSamplerGroup(0, mSamplerGroup);
}

}  // namespace Ethereal
