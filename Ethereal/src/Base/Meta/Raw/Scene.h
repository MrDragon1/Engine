#pragma once
#include "Base/Meta/Reflection.h"
#include "Core/Scene/Components.h"

namespace Ethereal {
    REFLECTION_TYPE(EntityRaw)
    CLASS(EntityRaw, Fields) {
        REFLECTION_BODY(EntityRaw);
      public:
        std::string Name;
        std::vector<Reflection::ReflectionPtr<Component>> Components;
    };


    REFLECTION_TYPE(SceneRaw)
    CLASS(SceneRaw, Fields) {
        REFLECTION_BODY(SceneRaw);
      public:
        std::string Name;
        std::vector<EntityRaw> Entities;
    };

}