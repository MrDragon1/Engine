#pragma once

#include "Core/Renderer/Environment.h"
#include "Core/Renderer/MaterialAsset.h"
#include "Backend/DriverApi.h"
#include "TypedUniform.h"
#include "UibGenerator.h"
#include "Utils/BitmaskEnum.h"
namespace Ethereal {

struct CameraParam {
    Matrix4 ViewProjectionMatrix;
    Matrix4 InverseViewProjectionMatrix;
    Matrix4 ProjectionMatrix;
    Matrix4 InverseProjectionMatrix;
    Matrix4 ViewMatrix;
    Matrix4 InverseViewMatrix;
    float FarPlane;
    float NearPlane;
    Vector3 CameraPosition;
};

struct EditorParam {
    uint32_t EntityID;
};
struct SceneParam {
    Vector3 CameraPosition;
    float EnvironmentMapIntensity;
    Vector4 ScissorNormalized;

    Ref<Environment> Environment;
};

struct LightParam {
    Vector3 Direction = Math::Normalize(Vector3(-0.236, 0.056, -0.514));
    float ShadowAmount;
    Vector3 Radiance = Vector3(1.0, 1.0, 1.0);
    float Multiplier = 1;
};

struct RenderPrimitiveParam {
    Matrix4 ModelMatrix;
};

struct BoneParam {
    Matrix4 BoneTransform[100];
};

class UniformManager : public RefCounted {
   public:
    UniformManager(const Ref<Backend::DriverApi>& api) : mApi(api) { Init(); };
    void Init();

    void UpdateFog();
    void UpdateCamera();
    void UpdateShadow(TextureHandle ShadowMap);
    void UpdateEditor();
    void UpdateScene();
    void UpdateLight();
    void UpdateMaterial(Ref<MaterialAsset> mat);
    void UpdateRenderPrimitive(RenderPrimitiveParam param);
    void UpdateBone();

    void Commit();
    void Bind();

   private:
    Ref<Backend::DriverApi> mApi;

    TypedUniform<ViewUib> mViewUib;
    TypedUniform<ShadowUib> mShadowUib;
    TypedUniform<LightUib> mLightUib;
    TypedUniform<RenderPrimitiveUib> mRenderPrimitiveUib;
    TypedUniform<RenderPrimitiveBoneUib> mRenderPrimitiveBoneUib;

    Ref<BufferObject> mViewUB;
    Ref<BufferObject> mShadowUB;
    Ref<BufferObject> mLightUB;
    Ref<BufferObject> mRenderPrimitiveUB;
    Ref<BufferObject> mRenderPrimitiveBoneUB;

    SamplerGroupHandle mSamplerGroup;
    SamplerGroupDescriptor mSamplerGroupDesc;
};

}  // namespace Ethereal
