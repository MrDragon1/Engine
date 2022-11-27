#pragma once
#include "Asset.h"

#include <filesystem>
namespace Ethereal
{

    struct AssetMetaData {
        AssetHandle Handle = 0;
        AssetType Type;

        std::filesystem::path FilePath;
        bool IsDataLoaded = false;
        bool IsMemoryAsset = false;

        bool IsValid() const { return Handle != 0 && !IsMemoryAsset; }
    };
}  // namespace Ethereal
