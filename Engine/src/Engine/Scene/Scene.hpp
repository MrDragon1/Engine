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

      private:
        entt::registry m_Registry;
        friend class Entity;
    };
}  // namespace Engine