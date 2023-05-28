#pragma once
#include "Core/Material/MaterialCore.h"

namespace Ethereal {
class MaterialManager {
   public:
    MaterialManager() = default;
    static MaterialCorePtr LoadMaterial(const string& path);
    static void LoadLibrary();

   private:
    static DocumentPtr mLibrary;

   private:
};
};  // namespace Ethereal
