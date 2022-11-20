#pragma once
#include "Core/Asset/AssetMetaData.h"

namespace Ethereal
{
    class AssetImporter {
      public:
        static bool TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset);
    };

}  // namespace Ethereal
