#include "RenderScene.hpp"
#include "RenderResource.hpp"
#include "RenderCommand.hpp"
namespace Ethereal
{

    RenderScene::RenderScene(const std::string& shaderPath) {
        m_Shader = Shader::Create(shaderPath);
        m_Shader->Bind();

        int32_t samplers[MaxTextureSlots];
        for (uint32_t i = 0; i < MaxTextureSlots; i++) samplers[i] = i;
        m_Shader->SetIntArray("u_Textures", samplers, MaxTextureSlots);
        
    }

    void RenderScene::UpdateVisiableMeshNode(const Ref<RenderResource>& renderResource) {
        m_Visiable_RenderMeshNode.clear();
        for (auto& gameObject : m_GameObjects) {
            for (auto& renderEntity : gameObject->m_RenderEntities) {
                auto& mesh = renderResource->GetGLMesh(renderEntity);
                auto& material = renderResource->GetGLMaterial(renderEntity);
                RenderMeshNode node;
                node.model_matrix = renderEntity.m_Transform_Desc.GetTransform();
                node.ref_mesh = &mesh;
                node.ref_material = &material;
                m_Visiable_RenderMeshNode.push_back(node);
            }
        }
    }

    void RenderScene::AddGameObject(Ref<GameObject>& gameObject) { m_GameObjects.push_back(gameObject); }

    void RenderScene::BeginRender(const EditorCamera& camera) {
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", camera.GetViewProjection());
        // Bind textures
        // for (uint32_t i = 0; i < TextureSlotIndex; i++) TextureSlots[i]->Bind(i);
        if (m_Visiable_RenderMeshNode.empty()) return;
        for (auto& RenderMeshNode : m_Visiable_RenderMeshNode) {
            RenderMeshNode.ref_mesh->m_VAO->Bind();
            RenderMeshNode.ref_material->m_BaseColorMap->Bind(0);
            RenderCommand::DrawIndexed(RenderMeshNode.ref_mesh->m_VAO, RenderMeshNode.ref_mesh->m_IndexCount);
            // RenderCommand::Draw(RenderMeshNode.ref_mesh->m_VAO, RenderMeshNode.ref_mesh->m_VertexCount);
        }
    }

    void RenderScene::BeginRender(const glm::mat4& projection, const glm::mat4& transform) {
        m_Shader->Bind();
        m_Shader->SetMat4("u_ViewProjection", projection * glm::inverse(transform));
        if (m_Visiable_RenderMeshNode.empty()) return;
        for (auto& RenderMeshNode : m_Visiable_RenderMeshNode) {
            RenderCommand::DrawIndexed(RenderMeshNode.ref_mesh->m_VAO, RenderMeshNode.ref_mesh->m_IndexCount);
            // RenderCommand::Draw(RenderMeshNode.ref_mesh->m_VAO, RenderMeshNode.ref_mesh->m_VertexCount);
        }
    }

    void RenderScene::Clear() {
        m_Visiable_RenderMeshNode.clear();
        m_GameObjects.clear();
    }

}  // namespace Ethereal