#pragma once

#include "Core/Scene/Entity.h"

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
        virtual void OnUpdate(TimeStamp ts) {}
        virtual void OnCreate() {}
        virtual void OnDestroy() {}

      private:
        Entity m_Entity;
        friend class Scene;
        friend class RenderSystem;
    };
}  // namespace Ethereal