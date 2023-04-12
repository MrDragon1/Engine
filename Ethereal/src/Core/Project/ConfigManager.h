#pragma once
#include "Core/Renderer/Uniform/UniformManager.h"
namespace Ethereal {
class ConfigManager {
   public:
    static struct CSMConfig {
        uint32_t ShadowMapSize = 4096;
        Vector4 CascadeSplits[16];
        Matrix4 DirLightMatrices[16];
        uint32_t CascadeCount = 4;

        // TODO: maybe use another camera for csm
        float NearPlane;
        float FarPlane;
        Matrix4 ViewMatrix;

        float FOV;
        float AspectRatio;
        Ref<Texture> ShadowMap;
    } sCSMConfig;

    static struct FogConfig {
        Vector4 FogColor;
        float FogStart;
        float FogDensity;
        float FogHeight;
        float FogHeightFalloff;
        float FogScatteringStart;
        float FogScatteringSize;
        bool FogEnable;
        bool FogFromIBL;
    } sFogConfig;

    static struct BloomConfig {
        bool Enabled = true;
        float Intensity = 1.0;
        float Threshold = 1.0;
        float Knee = 0.1;
    } sBloomConfig;

    static struct UniformManagerConfig {
        CameraParam CameraParam;
        EditorParam EditorParam;
        SceneParam SceneParam;
        LightParam LightParam;
        RenderPrimitiveParam RenderPrimitiveParam;
        BoneParam BoneParam;
    } sUniformManagerConfig;
};
}  // namespace Ethereal