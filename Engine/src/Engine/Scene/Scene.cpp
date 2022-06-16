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
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group) {
            const auto& [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);
            Renderer2D::DrawQuad(transform.Transform, sprite.Color);
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