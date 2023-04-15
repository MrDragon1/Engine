#pragma once

#include "Utils/EngineMarco.h"
#include "Core/Renderer/Mesh.h"
#include "Core/Renderer/RenderPass.h"
#include "Core/Renderer/RenderPass/BloomRenderPass.h"
#include "Core/Renderer/RenderPass/CSMRenderPass.h"
#include "Core/Renderer/RenderPass/EnvironmentMapRenderPass.h"
#include "Core/Renderer/RenderPass/MainCameraRenderPass.h"
#include "Core/Renderer/RenderPass/ShadowMapRenderPass.h"
#include "Core/Scene/Scene.h"

// For test backend
#include "Uniform/TypedUniform.h"
#include "Uniform/UibGenerator.h"
namespace Ethereal {
// TODO: Move this to RenderResource
struct BuildinData {
    Ref<Environment> Environment;
};

class RenderSystem : public RefCounted {
   public:
    RenderSystem();
    void Init();
    void Draw(TimeStamp ts);

    std::pair<Ref<Texture>, Ref<Texture>> CreateEnvironmentMap(const std::string& path);
    void SubmitStaticMesh(Ref<StaticMesh> staticMesh, Ref<MaterialTable> materialTabel, uint32_t EntityID,
                          const Matrix4& transform = Matrix4::IDENTITY);
    void SubmitMesh(Ref<Mesh> mesh, Ref<MaterialTable> materialTabel, uint32_t EntityID, const Matrix4& transform = Matrix4::IDENTITY);
    void SubmitRenderSceneData();
    void OnResize();
    void LoadProjectSettings();
    void UpdateUniformData();
    Ref<Environment> GetDefaultEnvironment() { return mBuildinData->Environment; }

    uint32_t GetMainImageHeight() { return mHeight; };
    uint32_t GetMainImageWidth() { return mWidth; };
    uint64_t GetMainImage();
    uint64_t GetSkyboxImage();
    int GetMousePicking(int x, int y);

    Ref<CSMRenderPass> GetCSMRenderPass() { return mCSMRenderPass; }
    Ref<Environment> GetEnv() { return mEnvironment; }

   private:
    Ref<MainCameraRenderPass> mMainCameraRenderPass;
    Ref<ShadowMapRenderPass> mShadowMapRenderPass;  // Don't use it for now.
    Ref<EnvironmentMapRenderPass> mEnvironmentMapRenderPass;
    Ref<BloomRenderPass> mBloomRenderPass;
    Ref<CSMRenderPass> mCSMRenderPass;

    Ref<Environment> mEnvironment;
    Ref<Texture> mMainImage;
    uint32_t mHeight, mWidth;

    DrawLists* mDrawLists;
    BuildinData* mBuildinData;
};
}  // namespace Ethereal