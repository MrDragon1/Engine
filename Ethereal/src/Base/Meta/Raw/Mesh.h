#pragma once

#include "Base/Meta/Reflection.h"
#include "Base/UUID.h"
namespace Ethereal {
    REFLECTION_TYPE(MaterialTableRaw)
    CLASS(MaterialTableRaw, Fields) {
        REFLECTION_BODY(MaterialTableRaw);
      public:
        std::vector<UUID> Materials;
    };

}
