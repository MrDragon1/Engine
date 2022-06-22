#pragma once

#include "entt/entt.hpp"

#include "Engine/Core/Timestep.hpp"

namespace Engine
{
    class Entity;
    class Scene {
      public:
        Scene();
        ~Scene();

        void OnUpdate(Timestep ts);

        entt::registry& Reg() { return m_Registry; }
        Entity CreateEntity(const std::string& name = std::string());
        void OnViewportResize(uint32_t width, uint32_t height);

      private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;
        friend class Entity;
        friend class SceneHierarchyPanel;
        
    };
}  // namespace Engine