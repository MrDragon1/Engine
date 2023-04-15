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
    mWidth = 1280;
    mHeight = 720;

    mDrawLists = &RenderPass::mDrawLists;

    mBuildinData = new BuildinData();

    mEnvironmentMapRenderPass = Ref<EnvironmentMapRenderPass>::Create();
    mEnvironmentMapRenderPass->Init(mWidth, mHeight);
    mMainCameraRenderPass = Ref<MainCameraRenderPass>::Create();
    mMainCameraRenderPass->Init(mWidth, mHeight);
    mShadowMapRenderPass = Ref<ShadowMapRenderPass>::Create();
    mShadowMapRenderPass->Init(mWidth, mHeight);
    mBloomRenderPass = Ref<BloomRenderPass>::Create();
    mBloomRenderPass->Init(mWidth, mHeight);
    mCSMRenderPass = Ref<CSMRenderPass>::Create();
    mCSMRenderPass->Init(mWidth, mHeight);

    // Must after mEnvironmentMapRenderPass Init
    mBuildinData->Environment = AssetManager::GetAsset<Environment>("skyboxs/Newport_Loft_Ref.hdr");

    mMainImage = mMainCameraRenderPass->mRenderTarget->color[0];
}

void RenderSystem::Draw(TimeStamp ts) {
    LoadProjectSettings();
    // mMainCameraRenderPass->mFramebuffer->Bind();
    // RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    // RenderCommand::Clear();
    mMainImage = mMainCameraRenderPass->mRenderTarget->color[0];
    mCSMRenderPass->Draw();

    mMainCameraRenderPass->Draw();

    // mMainCameraRenderPass->mFramebuffer->Unbind();
    // mMainImage = mMainCameraRenderPass->mFramebuffer->GetColorAttachment(0);

    //// mShadowMapRenderPass->SetLightPosition();
    // mCSMRenderPass->Draw();

    //// mShadowMapRenderPass->Draw();
    //// mShadowMapRenderPass->mFramebuffer->GetDepthAttachment()->Bind(5);

    // mMainCameraRenderPass->SetCSMData(mCSMRenderPass->GetData());
    //// TODO: Radiance and Irradiance may used as reversed wrongly in shader!!!
    // mEnvironment->RadianceMap->Bind(17);
    // mEnvironment->IrradianceMap->Bind(16);
    // RenderResource::BRDFLutTexture->Bind(15);
    // mMainCameraRenderPass->Draw();

    //// TODO : make skybox render pass a subpass of main camera render pass
    // mMainCameraRenderPass->mFramebuffer->Bind();
    // mEnvironment->IrradianceMap->Bind(0);
    // mSkyboxRenderPass->Draw();
    // mMainCameraRenderPass->mFramebuffer->Unbind();

    // For postprocessing
    if (Project::GetConfigManager().sBloomConfig.Enabled) {
        mBloomRenderPass->SetMainImage(mMainImage);
        mBloomRenderPass->Draw();

        mMainImage = mBloomRenderPass->GetBloomImage();
    }

    mDrawLists->MeshTransformMap.clear();
    mDrawLists->StaticMeshDrawList.clear();
    mDrawLists->MeshDrawList.clear();
}

void RenderSystem::OnResize() {
    mHeight = GlobalContext::GetViewportSize().y;
    mWidth = GlobalContext::GetViewportSize().x;
    mShadowMapRenderPass->OnResize(mWidth, mHeight);
    mMainCameraRenderPass->OnResize(mWidth, mHeight);
}

uint64_t RenderSystem::GetMainImage() { return GlobalContext::GetDriverApi()->GetTextueID(mMainImage); }

uint64_t RenderSystem::GetSkyboxImage() { return GlobalContext::GetDriverApi()->GetTextueID(mEnvironmentMapRenderPass->mInputTexture); };

int RenderSystem::GetMousePicking(int x, int y) { return mMainCameraRenderPass->GetMousePicking(x, y); }

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
        auto& transformStorage = mDrawLists->MeshTransformMap[meshKey].Transforms.emplace_back();
        transformStorage.Transform = submeshTransform;
        // Main geo
        {
            bool isTransparent = material->IsTransparent();
            if (isTransparent) ET_CORE_WARN("Only support untransparent material");
            auto& destDrawList = mDrawLists->StaticMeshDrawList;
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
        auto& transformStorage = mDrawLists->MeshTransformMap[meshKey].Transforms.emplace_back();
        transformStorage.Transform = submeshTransform;
        // Main geo
        {
            bool isTransparent = material->IsTransparent();
            if (isTransparent) ET_CORE_WARN("Only support untransparent material");
            auto& destDrawList = mDrawLists->MeshDrawList;
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
    mEnvironmentMapRenderPass->mInputTexture = ResourceLoader::LoadTexture(path);

    mEnvironmentMapRenderPass->Draw();
    // TODO: Copy the Ref<TextureCube> otherwise every Environment will point to the same Ref
    return {mEnvironmentMapRenderPass->mEnvironmentCubeMap, mEnvironmentMapRenderPass->mReflectionCubeMap};
}

void RenderSystem::LoadProjectSettings() {}

void RenderSystem::UpdateUniformData() {
    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();
}

}  // namespace Ethereal