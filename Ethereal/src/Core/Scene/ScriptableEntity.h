#pragma once

#include "Core/Scene/Entity.h"

namespace Ethereal
{
    class ScriptableEntity {
      public:
        virtual ~ScriptableEntity() = default;

        template <typename T>
        T& GetComponent() {
            return mEntity.GetComponent<T>();
        }

      protected:
        virtual void OnUpdate(TimeStamp ts) {}
        virtual void OnCreate() {}
        virtual void OnDestroy() {}

      private:
        Entity mEntity;
        friend class Scene;
        friend class RenderSystem;
    };
}  // namespace Ethereal