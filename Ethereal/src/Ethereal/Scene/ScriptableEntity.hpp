#pragma once

#include "Entity.hpp"

namespace Ethereal
{
    class ScriptableEntity {
      public:
        virtual ~ScriptableEntity() = default;

        template <typename T>
        T& GetComponent() {
            return m_Entity.GetComponent<T>();
        }

      protected:
        virtual void OnUpdate(Timestep ts) {}
        virtual void OnCreate() {}
        virtual void OnDestroy() {}

      private:
        Entity m_Entity;
        friend class Scene;
    };
}  // namespace Ethereal