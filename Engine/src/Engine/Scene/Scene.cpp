#include "Scene.hpp"

#include "Components.hpp"
#include "Engine/Renderer/Renderer2D.hpp"
namespace Engine
{
    Scene::Scene() {
    }

    Scene::~Scene() {
    }

    void Scene::OnUpdate(Timestep ts) {
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform.Transform, sprite.Color);
        }
    }

    entt::entity Scene::CreateEntity() {
        return m_Registry.create();
    }
}  // namespace Engine