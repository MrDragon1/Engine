#pragma once
#include "Core/Asset/AssetManager.h"

namespace Ethereal {
    class ResourceImporter {
      public:
        static bool Import(const std::string& filepath);

        static bool ImportModel(AssetHandle& handle, AssetMetaData& metaData);
    };
}
