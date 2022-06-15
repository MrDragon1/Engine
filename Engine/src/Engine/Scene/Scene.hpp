#pragma once

#include "entt/entt.hpp"

#include "Engine/Core/Timestep.hpp"

namespace Engine
{
    class Scene {
      public:
        Scene();
        ~Scene();

        void OnUpdate(Timestep ts);

        entt::registry& Reg() { return m_Registry; }
        entt::entity CreateEntity();
      private:
        entt::registry m_Registry;
    };
}  // namespace Engine