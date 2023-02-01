#include "RenderSystem.h"

#include "Ethereal/src/Core/Scene/Components.h"
#include "Ethereal/src/Core/Scene/Entity.h"
#include "Ethereal/src/Core/Scene/Scene.h"
#include "Ethereal/src/Core/Scene/ScriptableEntity.h"
#include "Ethereal/src/Base/GlobalContext.h"
#include "Core/Asset/AssetManager.h"
#include "Core/Renderer/RenderResource.h"
// Temporary
#include "Utils/AssetLoader.h"
#include "Resource/ResourceLoader.h"
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

        m_EnvironmentMapRenderPass = Ref<EnvironmentMapRenderPass>::Create();
        m_EnvironmentMapRenderPass->Init(m_Width, m_Height);
        m_MainCameraRenderPass = Ref<MainCameraRenderPass>::Create();
        m_MainCameraRenderPass->Init(m_Width, m_Height);
        m_ShadowMapRenderPass = Ref<ShadowMapRenderPass>::Create();
        m_ShadowMapRenderPass->Init(m_Width, m_Height);
        m_SkyboxRenderPass = Ref<SkyboxRenderPass>::Create();
        m_SkyboxRenderPass->Init(m_Width, m_Height);
        m_BloomRenderPass = Ref<BloomRenderPass>::Create();
        m_BloomRenderPass->Init(m_Width, m_Height);
        m_CSMRenderPass = Ref<CSMRenderPass>::Create();
        m_CSMRenderPass->Init(m_Width, m_Height);

        // Must after m_EnvironmentMapRenderPass Init
        m_BuildinData->Environment = AssetManager::GetAsset<Environment>("skyboxs/Newport_Loft_Ref.hdr");

        m_MainImage = m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0);

        m_UniformBufferSet = UniformBufferSet::Create(1);
        m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(CameraData), 0), 0);
        m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(ShadowData), 1), 0);
        m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(SceneData), 2), 0);
        m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(RendererData), 3), 0);
    }

    void RenderSystem::Draw(TimeStamp ts) {
        LoadProjectSettings();

        // m_ShadowMapRenderPass->SetLightPosition();
        m_CSMRenderPass->Draw();

        // m_ShadowMapRenderPass->Draw();
        // m_ShadowMapRenderPass->m_Framebuffer->GetDepthAttachment()->Bind(5);

        m_MainCameraRenderPass->SetCSMData(m_CSMRenderPass->GetData());
        m_Environment->RadianceMap->Bind(6);
        m_Environment->IrradianceMap->Bind(7);
        RenderResource::BRDFLutTexture->Bind(8);
        m_MainCameraRenderPass->Draw();

        // TODO : make skybox render pass a subpass of main camera render pass
        m_MainCameraRenderPass->m_Framebuffer->Bind();
        m_Environment->IrradianceMap->Bind(0);
        m_SkyboxRenderPass->Draw();
        m_MainCameraRenderPass->m_Framebuffer->Unbind();

        // For postprocessing
        auto mainImg = m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0);
        m_MainImage = mainImg;
        if(m_BloomRenderPass->GetEnabled()) {
            m_BloomRenderPass->SetMainImage(mainImg);
            m_BloomRenderPass->Draw();

            m_MainImage = m_BloomRenderPass->GetBloomImage();
        }


        m_DrawLists->MeshTransformMap.clear();
        m_DrawLists->StaticMeshDrawList.clear();
    }

    void RenderSystem::OnResize() {
        m_Height = GlobalContext::GetViewportSize().y;
        m_Width = GlobalContext::GetViewportSize().x;
        m_ShadowMapRenderPass->OnResize(m_Width, m_Height);
        m_MainCameraRenderPass->OnResize(m_Width, m_Height);
    }

    uint64_t RenderSystem::GetMainImage() { return m_MainImage->GetRendererID(); }

    int RenderSystem::GetMousePicking(int x, int y) { return m_MainCameraRenderPass->GetMousePicking(x, y); }

    void RenderSystem::SubmitStaticMesh(Ref<StaticMesh> staticMesh, Ref<MaterialTable> materialTable, uint32_t EntityID, const Matrix4& transform) {
        Ref<MeshSource> meshSource = staticMesh->GetMeshSource();
        const auto& submeshData = meshSource->GetSubmeshes();
        for (uint32_t submeshIndex : staticMesh->GetSubmeshes()) {
            Matrix4 submeshTransform = transform * submeshData[submeshIndex].Transform;

            const auto& submeshes = staticMesh->GetMeshSource()->GetSubmeshes();
            uint32_t materialIndex = submeshes[submeshIndex].MaterialIndex;
            Ref<MaterialAsset> material = materialTable->GetMaterial(materialIndex);
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
                dc.InstanceCount++;
            }
        }
    }

    void RenderSystem::SubmitMesh(Ref<Mesh> mesh, Ref<MaterialTable> materialTable, uint32_t EntityID, const Matrix4& transform) {
        Ref<MeshSource> meshSource = mesh->GetMeshSource();
        const auto& submeshData = meshSource->GetSubmeshes();
        for (uint32_t submeshIndex : mesh->GetSubmeshes()) {
            Matrix4 submeshTransform = transform * submeshData[submeshIndex].Transform;

            const auto& submeshes = mesh->GetMeshSource()->GetSubmeshes();
            uint32_t materialIndex = submeshes[submeshIndex].MaterialIndex;
            Ref<MaterialAsset> material = materialTable->GetMaterial(materialIndex);
            AssetHandle materialHandle = material->Handle;

            MeshKey meshKey = {mesh->Handle, materialHandle, submeshIndex, false, EntityID};
            auto& transformStorage = m_DrawLists->MeshTransformMap[meshKey].Transforms.emplace_back();
            transformStorage.Transform = submeshTransform;
            // Main geo
            {
                bool isTransparent = material->IsTransparent();
                if (isTransparent) ET_CORE_WARN("Only support untransparent material");
                auto& destDrawList = m_DrawLists->MeshDrawList;
                auto& dc = destDrawList[meshKey];
                dc.Mesh = mesh;
                dc.SubmeshIndex = submeshIndex;
                dc.MaterialTable = materialTable;
                dc.InstanceCount++;
            }
        }
    }

    void RenderSystem::SubmitRenderSceneData(const ShaderCommonData& data) {
        //从editor传过来的数据在此写入UBO
        m_UniformBufferSet->Get(0, 0)->SetData(&data.CameraData, sizeof(CameraData));
        m_UniformBufferSet->Get(0, 1)->SetData(&data.ShadowData, sizeof(ShadowData));
        m_UniformBufferSet->Get(0, 2)->SetData(&data.SceneData, sizeof(SceneData));
        m_UniformBufferSet->Get(0, 3)->SetData(&data.RendererData, sizeof(RendererData));

        m_MainCameraRenderPass->SetCameraPosition(data.RenderSceneData.CameraPosition);

        m_CSMRenderPass->SetViewMatrix(data.CameraData.ViewMatrix);
        m_CSMRenderPass->SetNearFarPlane(data.RenderSceneData.NearPlane, data.RenderSceneData.FarPlane);
        m_CSMRenderPass->SetFOV(data.RenderSceneData.FOV);
        m_CSMRenderPass->SetAspectRatio(data.RenderSceneData.AspectRatio);
        m_CSMRenderPass->SetLightDir(data.RenderSceneData.DirectionalLightDir);

        m_Environment = data.RenderSceneData.Environment;

        m_MainCameraRenderPass->SetViewProjectionMatrix(data.CameraData.ViewProjectionMatrix);
    }

    std::pair<Ref<TextureCube>, Ref<TextureCube>> RenderSystem::CreateEnvironmentMap(const std::string& path) {
        m_EnvironmentMapRenderPass->m_Path = path;
        m_EnvironmentMapRenderPass->Draw();
        // TODO: Copy the Ref<TextureCube> otherwise every Environment will point to the same Ref
        return {m_EnvironmentMapRenderPass->m_EnvironmentCubeMap, m_EnvironmentMapRenderPass->m_ReflectionCubeMap};
    }

    void RenderSystem::LoadProjectSettings() {
        // Bloom Settings
        m_BloomRenderPass->GetEnabled() = Project().GetSettings().bloomSetting.enabled;
        m_BloomRenderPass->GetIntensity() = Project().GetSettings().bloomSetting.intensity;
        m_BloomRenderPass->GetThreshold() = Project().GetSettings().bloomSetting.threshold;
        m_BloomRenderPass->GetKnee() = Project().GetSettings().bloomSetting.knee;
    }
}  // namespace Ethereal