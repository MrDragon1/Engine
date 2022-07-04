#pragma once

#include "entt/entt.hpp"

#include "Engine/Core/Timestep.hpp"
#include "Engine/Renderer/EditorCamera.hpp"

class b2World;
namespace Engine
{

    class Entity;
    class Scene {
      public:
        Scene();
        ~Scene();

        void OnUpdateRuntime(Timestep ts);
        void OnUpdateEditor(Timestep ts, EditorCamera& camera);

        Entity CreateEntity(const std::string& name = std::string());
        void DestroyEntity(Entity entity);
        void OnRuntimeStart();
        void OnRuntimeStop();

        void OnViewportResize(uint32_t width, uint32_t height);
        Entity GetPrimaryCameraEntity();

      private:
        template <typename T>
        void OnComponentAdded(Entity entity, T& component);

      private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        b2World* m_PhysicsWorld = nullptr;

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };
}  // namespace Engine