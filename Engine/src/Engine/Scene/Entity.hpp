#pragma once
#include "Scene.hpp"
#include "entt/entt.hpp"

namespace Engine
{
    class Entity {
      public:
        Entity() = default;
        Entity(const Entity&) = default;
        Entity(entt::entity handle, Scene* Scene);

        template <typename T, typename... Args>
        T& AddComponent(Args&&... args) {
            ENGINE_CORE_ASSERT(!HasComponent<T>(), "Entity already has component!");
            return m_Scene->m_Registry.emplace<T>(m_Entity, std::forward<Args>(args)...);
        }

        template <typename T>
        T& GetComponent() {
            ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            return m_Scene->m_Registry.get<T>(m_Entity);
        }

        template <typename T>
        bool HasComponent() {
            return m_Scene->m_Registry.any_of<T>(m_Entity);
        }

        template <typename T>
        void RemoveComponent() {
            ENGINE_CORE_ASSERT(HasComponent<T>(), "Entity does not have component!");
            m_Scene->m_Registry.remove<T>(m_Entity);
        }

        operator bool() const { return m_Entity != entt::null; }
		operator uint32_t() const { return (uint32_t)m_Entity; }

		bool operator==(const Entity& other) const
		{
			return m_Entity == other.m_Entity && m_Scene == other.m_Scene;
		}

		bool operator!=(const Entity& other) const
		{
			return !(*this == other);
		}
      private:
        entt::entity m_Entity = entt::null;
        Scene* m_Scene = nullptr;
    };
}  // namespace Engine