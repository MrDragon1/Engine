#include "RenderScene.hpp"
#include "RenderResource.hpp"
#include "RenderCommand.hpp"
namespace Ethereal
{
    RenderScene::RenderScene(const std::string& shaderPath) {
        m_Shader = Shader::Create(shaderPath);
    }

    void RenderScene::UpdateVisiableMeshNode(const Ref<RenderResource>& renderResource) {
        m_Visiable_RenderMeshNode.clear();
        for (auto& gameObject : m_GameObjects) {
            for (auto& renderEntity : gameObject->m_RenderEntities) {
                auto& mesh = renderResource->GetGLMesh(renderEntity);
                RenderMeshNode node;
                node.model_matrix = renderEntity.m_Transform_Desc.GetTransform();
                node.ref_mesh = &mesh;
                m_Visiable_RenderMeshNode.push_back(node);
            }
        }
    }

    void RenderScene::AddGameObject(Ref<GameObject>& gameObject) { m_GameObjects.push_back(gameObject); }

    void RenderScene::BeginRender(const EditorCamera& camera) {
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", camera.GetViewProjection());
        if (m_Visiable_RenderMeshNode.empty()) return;
        for (auto& RenderMeshNode : m_Visiable_RenderMeshNode) {
            RenderCommand::DrawIndexed(RenderMeshNode.ref_mesh->m_VAO, RenderMeshNode.ref_mesh->m_IndexCount);
        }
    }
    
    void RenderScene::BeginRender(const glm::mat4& projection, const glm::mat4& transform)
    {
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", projection * glm::inverse(transform));
        if (m_Visiable_RenderMeshNode.empty()) return;
        for (auto& RenderMeshNode : m_Visiable_RenderMeshNode) {
            RenderCommand::DrawIndexed(RenderMeshNode.ref_mesh->m_VAO, RenderMeshNode.ref_mesh->m_IndexCount);
        }
    }

    void RenderScene::Clear() {
        m_Visiable_RenderMeshNode.clear();
        m_GameObjects.clear();
    }

}  // namespace Ethereal