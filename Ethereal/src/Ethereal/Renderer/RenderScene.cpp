#include "RenderScene.hpp"
#include "RenderResource.hpp"
#include "RenderCommand.hpp"
#include "RenderPass.hpp"
namespace Ethereal
{
    void RenderScene::UpdateVisiableMeshNode(Ref<RenderResource>& renderResource) {
        m_MainCameraVisiableNodes.clear();
        for (auto& gameObject : m_GameObjects) {
            for (auto& renderEntity : gameObject->m_RenderEntities) {
                auto& mesh = renderResource->GetGLMesh(renderEntity);
                auto& material = renderResource->GetGLMaterial(renderEntity);
                RenderNode node;
                node.EntityID = renderEntity.m_InstanceID;
                node.model_matrix = renderEntity.m_Transform_Desc.GetTransform();
                node.ref_mesh = &mesh;
                node.ref_material = &material;
                m_MainCameraVisiableNodes.push_back(node);
            }
        }
    }

    void RenderScene::AddGameObject(const Ref<GameObject>& gameObject) { m_GameObjects.push_back(gameObject); }

    void RenderScene::Clear() {
        m_MainCameraVisiableNodes.clear();
        m_GameObjects.clear();
    }
    
    void RenderScene::SetVisiableNodeReference()
    {
        RenderPass::m_VisiableNodes.p_main_camera_visible_mesh_nodes = &m_MainCameraVisiableNodes;
    }

}  // namespace Ethereal