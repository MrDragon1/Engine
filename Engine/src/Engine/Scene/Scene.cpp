#include "Scene.hpp"

#include "Entity.hpp"
#include "Components.hpp"
#include "Engine/Renderer/Renderer2D.hpp"

namespace Engine
{
    Scene::Scene() {
    }

    Scene::~Scene() {
    }

    void Scene::OnUpdate(Timestep ts) {
        Camera* mainCamera = nullptr;
        glm::mat4* cameraTransform = nullptr;
        {
            auto group = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : group) {
                const auto& [transform, camera] = group.get<TransformComponent, CameraComponent>(entity);
                if (camera.Primary) {
                    //ENGINE_CORE_INFO("Found primary camera " + m_Registry.get<TagComponent>(entity).Tag);
                    mainCamera = &camera.Camera;
                    cameraTransform = &transform.Transform;
                    break;
                }
            }
        }

        if (mainCamera) {
            Renderer2D::BeginScene(mainCamera->GetProjection(), *cameraTransform);

            auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
            for (auto entity : group) {
                const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
                Renderer2D::DrawQuad(transform, sprite.Color);
            }

            Renderer2D::EndScene();
        }
    }

    Entity Scene::CreateEntity(const std::string& name) {
        Entity entity = {m_Registry.create(), this};
        entity.AddComponent<TransformComponent>();
        auto& tag = entity.AddComponent<TagComponent>();
        tag.Tag = name.empty() ? "DefaultEntity" : name;
        return entity;
    }

}  // namespace Engine