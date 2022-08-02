#pragma once
#include "Scene.h"
#include "Components.h"
#include "entt/entt.hpp"

namespace Ethereal
{
    class Entity {
      public:
        Entity() = default;
        Entity(const Entity&) = default;
        Entity(entt::entity handle, Scene* Scene);

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            ET_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            T& component = m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        template <typename T>
        T& GetComponent() {
            ET_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_Entity);
        }

        template <typename T>
        bool HasComponent() {
            return m_Scene->m_Registry.any_of<T>(m_Entity);
        }

        template <typename T>
        void RemoveComponent() {
            ET_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_Entity);
        }

        template <typename T, typename... Args>
        T& AddOrReplaceComponent(Args&&... args) {
            T& component = m_Scene->m_Registry.emplace_or_replace<T>(m_Entity, std::forward<Args>(args)...);
            m_Scene->OnComponentAdded<T>(*this, component);
            return component;
        }

        UUID GetUUID() { return GetComponent<IDComponent>().ID; };
        const std::string& GetName() { return GetComponent<TagComponent>().Tag; }

        operator bool() const { return m_Entity != entt::null && m_Scene != nullptr; }
        operator uint32_t() const { return (uint32_t)m_Entity; }
        operator entt::entity() const { return m_Entity; }

        bool operator==(const Entity& other) const { return m_Entity == other.m_Entity && m_Scene == other.m_Scene; }
        bool operator!=(const Entity& other) const { return !(*this == other); }

      private:
        entt::entity m_Entity = entt::null;
        Scene* m_Scene = nullptr;
    };
}  // namespace Ethereal