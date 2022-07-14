#include "RenderSystem.hpp"

#include "Ethereal/Scene/Components.hpp"
#include "Ethereal/Scene/Entity.hpp"
#include "Ethereal/Scene/Scene.hpp"
#include "Ethereal/Scene/ScriptableEntity.hpp"
// Temporary
#include "Ethereal/Utils/AssetLoader.hpp"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"

namespace Ethereal
{
    RenderSystem::RenderSystem() { Init(); }

    void RenderSystem::Init() {
        m_Width = 1280;
        m_Height = 720;
        m_RenderScene = CreateRef<RenderScene>();
        m_RenderResource = CreateRef<RenderResource>();
        m_MainCameraRenderPass = CreateRef<MainCameraRenderPass>();
        m_MainCameraRenderPass->Init(m_Width, m_Height);

        m_RenderScene->SetVisiableNodeReference();
    }

    void RenderSystem::Draw(Timestep ts) { m_MainCameraRenderPass->Draw(); }

    void RenderSystem::UpdateRenderScene(const RenderSceneData& renderSceneData) {
        m_RenderScene->Clear();
        for (const auto& data : renderSceneData.EntitiesData) {
            RenderEntity renderEntity;
            renderEntity.m_InstanceID = data.EntityID;
            renderEntity.m_Transform_Desc = data.Transform;
            renderEntity.m_Mesh_Desc = data.Mesh;
            renderEntity.m_Material_Desc = data.Material;
            // ET_CORE_INFO(renderEntity.m_Mesh_Desc.m_filePath);
            //* Load Mesh Data
            RenderMeshData renderMeshData;
            bool is_MeshLoaded = m_RenderScene->getMeshAssetIdAllocator().hasElement(renderEntity.m_Mesh_Desc);
            if (!is_MeshLoaded) {
                renderMeshData = m_RenderResource->LoadMeshData(renderEntity.m_Mesh_Desc, (int)(uint32_t)renderEntity.m_InstanceID);
            }
            renderEntity.m_MeshAssetID = m_RenderScene->getMeshAssetIdAllocator().allocUUID(renderEntity.m_Mesh_Desc);
            if (!is_MeshLoaded) {
                m_RenderResource->UploadRenderResource(renderEntity, renderMeshData);
            }

            //* Load Material Data
            bool is_MaterialLoaded = m_RenderScene->getMaterialAssetIdAllocator().hasElement(renderEntity.m_Material_Desc);
            if (!is_MaterialLoaded) {
                m_RenderResource->LoadMaterialData(renderEntity.m_Material_Desc);
            }
            renderEntity.m_MaterialAssetID = m_RenderScene->getMaterialAssetIdAllocator().allocUUID(renderEntity.m_Material_Desc);
            if (!is_MaterialLoaded) {
                m_RenderResource->UploadRenderResource(renderEntity, renderEntity.m_Material_Desc);
            }

            Ref<GameObject> gameObject = CreateRef<GameObject>();
            gameObject->AddRenderEntity(renderEntity);
            m_RenderScene->AddGameObject(gameObject);
        }
        m_RenderScene->UpdateVisiableMeshNode(m_RenderResource);

        m_MainCameraRenderPass->SetViewProjectionMatrix(renderSceneData.ViewProjectionMatrix);
    }

    void RenderSystem::OnResize(int width, int height) {
        m_Height = height;
        m_Width = width;
        m_MainCameraRenderPass->OnResize(width, height);
    }

    uint64_t RenderSystem::GetMainImage() { return m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0)->GetRendererID(); }

    int RenderSystem::GetMousePicking(int x, int y) { return m_MainCameraRenderPass->GetMousePicking(x, y); }
}  // namespace Ethereal