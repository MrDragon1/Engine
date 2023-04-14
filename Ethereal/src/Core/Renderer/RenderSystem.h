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
    Ref<Environment> GetDefaultEnvironment() { return m_BuildinData->Environment; }

    uint32_t GetMainImageHeight() { return m_Height; };
    uint32_t GetMainImageWidth() { return m_Width; };
    uint64_t GetMainImage();
    uint64_t GetSkyboxImage();
    int GetMousePicking(int x, int y);

    Ref<CSMRenderPass> GetCSMRenderPass() { return m_CSMRenderPass; }
    Ref<Environment> GetEnv() { return m_Environment; }

   private:
    Ref<MainCameraRenderPass> m_MainCameraRenderPass;
    Ref<ShadowMapRenderPass> m_ShadowMapRenderPass;  // Don't use it for now.
    Ref<EnvironmentMapRenderPass> m_EnvironmentMapRenderPass;
    Ref<BloomRenderPass> m_BloomRenderPass;
    Ref<CSMRenderPass> m_CSMRenderPass;

    Ref<Environment> m_Environment;
    Ref<Texture> m_MainImage;
    uint32_t m_Height, m_Width;

    DrawLists* m_DrawLists;
    BuildinData* m_BuildinData;
};
}  // namespace Ethereal