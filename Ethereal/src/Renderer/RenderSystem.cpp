#include "RenderSystem.h"

#include "Scene/Components.h"
#include "Scene/Entity.h"
#include "Scene/Scene.h"
#include "Scene/ScriptableEntity.h"
#include "Core/GlobalContext.h"
// Temporary
#include "Utils/AssetLoader.h"
#include "Utils/AssetManager.h"

#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"

namespace Ethereal
{
    RenderSystem::RenderSystem() {}

    void RenderSystem::Init() {
        m_Width = 1280;
        m_Height = 720;

        m_DrawLists = &RenderPass::m_DrawLists;
        m_BuildinData = new BuildinData();

        m_BuildinData->WhiteTexture = TextureManager::AddTexture("assets/buildin/textures/white.png");
        m_BuildinData->BlackTexture = TextureManager::AddTexture("assets/buildin/textures/black.png");
        m_BuildinData->BRDFLutTexture = TextureManager::AddTexture("assets/buildin/textures/brdf_schilk.hdr");
        m_BuildinData->Cube = Ref<StaticMesh>::Create(Ref<MeshSource>::Create("assets/meshes/source/cube.obj"));
        //        auto [radiance, irradiance] = CreateEnvironmentMap("assets/skyboxs/Newport_Loft/Newport_Loft_Env.hdr");
        //        m_BuildinData->Environment = Ref<Environment>::Create(radiance, irradiance);

        m_MainCameraRenderPass = Ref<MainCameraRenderPass>::Create();
        m_MainCameraRenderPass->Init(m_Width, m_Height);
        m_ShadowMapRenderPass = Ref<ShadowMapRenderPass>::Create();
        m_ShadowMapRenderPass->Init(m_Width, m_Height);
        m_SkyboxRenderPass = Ref<SkyboxRenderPass>::Create();
        m_SkyboxRenderPass->Init(m_Width, m_Height);
        m_EnvironmentMapRenderPass = Ref<EnvironmentMapRenderPass>::Create();
        m_EnvironmentMapRenderPass->Init(m_Width, m_Height);
    }

    void RenderSystem::Draw(Timestep ts) {
        // m_ShadowMapRenderPass->SetLightPosition();
        m_MainCameraRenderPass->SetLightSpaceMatrix(m_ShadowMapRenderPass->m_ViewProjectionMatrix);

        // m_ShadowMapRenderPass->Draw();
        // m_ShadowMapRenderPass->m_Framebuffer->GetDepthAttachment()->Bind(5);

        m_Environment->RadianceMap->Bind(6);
        m_Environment->IrradianceMap->Bind(7);
        GlobalContext::GetRenderSystem().GetBRDFLutTexture()->Bind(8);
        m_MainCameraRenderPass->Draw();

        // TODO : make skybox render pass a subpass of main camera render pass
        m_MainCameraRenderPass->m_Framebuffer->Bind();
        m_Environment->IrradianceMap->Bind(0);
        m_SkyboxRenderPass->Draw();
        m_MainCameraRenderPass->m_Framebuffer->Unbind();

        m_DrawLists->MeshTransformMap.clear();
        m_DrawLists->StaticMeshDrawList.clear();
    }

    void RenderSystem::OnResize() {
        m_Height = GlobalContext::GetViewportSize().y;
        m_Width = GlobalContext::GetViewportSize().x;
        m_ShadowMapRenderPass->OnResize(m_Width, m_Height);
        m_MainCameraRenderPass->OnResize(m_Width, m_Height);
    }

    uint64_t RenderSystem::GetMainImage() { return m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0)->GetRendererID(); }

    int RenderSystem::GetMousePicking(int x, int y) { return m_MainCameraRenderPass->GetMousePicking(x, y); }

    void RenderSystem::SubmitStaticMesh(Ref<StaticMesh> staticMesh, Ref<MaterialTable> materialTable, uint32_t EntityID, const glm::mat4& transform,
                                        Ref<Material> overrideMaterial) {
        Ref<MeshSource> meshSource = staticMesh->GetMeshSource();
        const auto& submeshData = meshSource->GetSubmeshes();
        for (uint32_t submeshIndex : staticMesh->GetSubmeshes()) {
            glm::mat4 submeshTransform = transform * submeshData[submeshIndex].Transform;

            const auto& submeshes = staticMesh->GetMeshSource()->GetSubmeshes();
            uint32_t materialIndex = submeshes[submeshIndex].MaterialIndex;
            Ref<MaterialAsset> material = materialTable->HasMaterial(materialIndex) ? materialTable->GetMaterial(materialIndex)
                                                                                    : staticMesh->GetMaterials()->GetMaterial(materialIndex);
            AssetHandle materialHandle = material->Handle;

            MeshKey meshKey = {staticMesh->Handle, materialHandle, submeshIndex, false, EntityID};
            auto& transformStorage = m_DrawLists->MeshTransformMap[meshKey].Transforms.emplace_back();
            transformStorage.Transform = submeshTransform;
            // Main geo
            {
                bool isTransparent = material->IsTransparent();
                if (isTransparent) ET_CORE_WARN("Only support untransparent material");
                auto& destDrawList = m_DrawLists->StaticMeshDrawList;
                auto& dc = destDrawList[meshKey];
                dc.StaticMesh = staticMesh;
                dc.SubmeshIndex = submeshIndex;
                dc.MaterialTable = materialTable;
                dc.OverrideMaterial = overrideMaterial;
                dc.InstanceCount++;
            }
        }
    }

    void RenderSystem::SubmitRenderSceneData(const RenderSceneData& data) {
        m_MainCameraRenderPass->SetCameraPosition(data.CameraPosition);
        m_SkyboxRenderPass->SetSkyboxProjection(data.ProjectionMatrix);
        m_SkyboxRenderPass->SetSkyboxView(data.ViewMatrix);
        m_Environment = data.Environment;

        m_MainCameraRenderPass->SetViewProjectionMatrix(data.ViewProjectionMatrix);
    }

    std::pair<Ref<TextureCube>, Ref<TextureCube>> RenderSystem::CreateEnvironmentMap(const std::string& path) {
        m_EnvironmentMapRenderPass->m_Path = path;
        m_EnvironmentMapRenderPass->Draw();
        // TODO: Copy the Ref<TextureCube> otherwise every Environment will point to the same Ref
        return {m_EnvironmentMapRenderPass->m_EnvironmentCubeMap, m_EnvironmentMapRenderPass->m_ReflectionCubeMap};
    }
}  // namespace Ethereal