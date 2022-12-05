#include "pch.h"
#include "Entity.h"


namespace Ethereal
{
    Entity::Entity(entt::entity handle, Scene* Scene) : m_Entity(handle), m_Scene(Scene) {}

    bool Entity::Load(const EntityRaw& raw) {
        for(auto component: raw.Components) {
            if(component.getTypeName() == "IDComponent") {
                auto id = dynamic_cast<IDComponent*>(component.getPtr());
                auto& c = AddComponent<IDComponent>(*id);
                c.PostLoad();
            }
            else if(component.getTypeName() == "TagComponent") {
                auto tag = dynamic_cast<TagComponent*>(component.getPtr());
                auto& c = AddComponent<TagComponent>(*tag);
                c.PostLoad();
            }
            else if(component.getTypeName() == "TransformComponent") {
                auto transform = dynamic_cast<TransformComponent*>(component.getPtr());
                auto& c = AddComponent<TransformComponent>(*transform);
                c.PostLoad();
            }
            else if(component.getTypeName() == "CameraComponent") {
                auto camera = dynamic_cast<CameraComponent*>(component.getPtr());
                auto& c = AddComponent<CameraComponent>(*camera);
                c.PostLoad();
            }
            else if(component.getTypeName() == "StaticMeshComponent") {
                auto mesh = dynamic_cast<StaticMeshComponent*>(component.getPtr());
                auto& c = AddComponent<StaticMeshComponent>(*mesh);
                c.PostLoad();
            }
            else if(component.getTypeName() == "MeshComponent") {
                auto light = dynamic_cast<MeshComponent*>(component.getPtr());
                auto& c = AddComponent<MeshComponent>(*light);
                c.PostLoad();
            }
        }
        return true;
    }

    bool Entity::Save(EntityRaw& raw) {
        if(HasComponent<IDComponent>()) {
            auto& c = GetComponent<IDComponent>();
            raw.Components.push_back(Reflection::ReflectionPtr<IDComponent>("IDComponent", &c));
        }
        if(HasComponent<TagComponent>()) {
            auto& c = GetComponent<TagComponent>();
            raw.Components.push_back(Reflection::ReflectionPtr<TagComponent>("TagComponent", &c));
        }
        if(HasComponent<TransformComponent>()) {
            auto& c = GetComponent<TransformComponent>();
            raw.Components.push_back(Reflection::ReflectionPtr<TransformComponent>("TransformComponent", &c));
        }
        if(HasComponent<CameraComponent>()) {
            auto& c = GetComponent<CameraComponent>();
            raw.Components.push_back(Reflection::ReflectionPtr<CameraComponent>("CameraComponent", &c));
        }
        if(HasComponent<StaticMeshComponent>()) {
            auto& c = GetComponent<StaticMeshComponent>();
            raw.Components.push_back(Reflection::ReflectionPtr<StaticMeshComponent>("StaticMeshComponent", &c));
        }
        if(HasComponent<MeshComponent>()) {
            auto& c = GetComponent<MeshComponent>();
            raw.Components.push_back(Reflection::ReflectionPtr<MeshComponent>("MeshComponent", &c));
        }
        return true;
    }

}  // namespace Ethereal