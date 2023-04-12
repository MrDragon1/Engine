#include "RenderSystem.h"

#include "Core/Asset/AssetManager.h"
#include "Core/Renderer/RenderResource.h"
#include "Base/GlobalContext.h"
#include "Core/Scene/Components.h"
#include "Core/Scene/Entity.h"
#include "Core/Scene/Scene.h"
#include "Core/Scene/ScriptableEntity.h"
// Temporary
#include "Resource/ResourceLoader.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"

// Test Backend
#include "Core/Renderer/Uniform/UibGenerator.h"

namespace Ethereal {
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
    m_BloomRenderPass = Ref<BloomRenderPass>::Create();
    m_BloomRenderPass->Init(m_Width, m_Height);
    m_CSMRenderPass = Ref<CSMRenderPass>::Create();
    m_CSMRenderPass->Init(m_Width, m_Height);

    // Must after m_EnvironmentMapRenderPass Init
    m_BuildinData->Environment = AssetManager::GetAsset<Environment>("skyboxs/Newport_Loft_Ref.hdr");

    m_MainImage = m_MainCameraRenderPass->mRenderTarget->color[0];
}

void RenderSystem::Draw(TimeStamp ts) {
    LoadProjectSettings();
    // m_MainCameraRenderPass->m_Framebuffer->Bind();
    // RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    // RenderCommand::Clear();
    m_MainImage = m_MainCameraRenderPass->mRenderTarget->color[0];
    m_CSMRenderPass->Draw();

    m_MainCameraRenderPass->Draw();

    // m_MainCameraRenderPass->m_Framebuffer->Unbind();
    // m_MainImage = m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0);

    //// m_ShadowMapRenderPass->SetLightPosition();
    // m_CSMRenderPass->Draw();

    //// m_ShadowMapRenderPass->Draw();
    //// m_ShadowMapRenderPass->m_Framebuffer->GetDepthAttachment()->Bind(5);

    // m_MainCameraRenderPass->SetCSMData(m_CSMRenderPass->GetData());
    //// TODO: Radiance and Irradiance may used as reversed wrongly in shader!!!
    // m_Environment->RadianceMap->Bind(17);
    // m_Environment->IrradianceMap->Bind(16);
    // RenderResource::BRDFLutTexture->Bind(15);
    // m_MainCameraRenderPass->Draw();

    //// TODO : make skybox render pass a subpass of main camera render pass
    // m_MainCameraRenderPass->m_Framebuffer->Bind();
    // m_Environment->IrradianceMap->Bind(0);
    // m_SkyboxRenderPass->Draw();
    // m_MainCameraRenderPass->m_Framebuffer->Unbind();

    // For postprocessing
    if (Project::GetConfigManager().sBloomConfig.Enabled) {
        m_BloomRenderPass->SetMainImage(m_MainImage);
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

uint64_t RenderSystem::GetMainImage() { return GlobalContext::GetDriverApi()->GetTextueID(m_MainImage); }

uint64_t RenderSystem::GetSkyboxImage() { return GlobalContext::GetDriverApi()->GetTextueID(m_EnvironmentMapRenderPass->mInputTexture); };

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

void RenderSystem::SubmitRenderSceneData() { UpdateUniformData(); }

std::pair<Ref<Texture>, Ref<Texture>> RenderSystem::CreateEnvironmentMap(const std::string& path) {
    m_EnvironmentMapRenderPass->mInputTexture = ResourceLoader::LoadTexture(path);

    m_EnvironmentMapRenderPass->Draw();
    // TODO: Copy the Ref<TextureCube> otherwise every Environment will point to the same Ref
    return {m_EnvironmentMapRenderPass->mEnvironmentCubeMap, m_EnvironmentMapRenderPass->mReflectionCubeMap};
}

void RenderSystem::LoadProjectSettings() {}

void RenderSystem::UpdateUniformData() {
    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();
}

}  // namespace Ethereal