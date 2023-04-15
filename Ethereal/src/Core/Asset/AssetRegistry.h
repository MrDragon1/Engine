#pragma once
#include "Core/Asset/AssetMetaData.h"

#include <unordered_map>

namespace Ethereal
{
    class AssetRegistry {
      public:
        AssetMetaData& operator[](const AssetHandle handle);
        const AssetMetaData& Get(const AssetHandle handle) const;

        size_t Count() const { return mAssetRegistry.size(); }
        bool Contains(const AssetHandle handle) const;
        size_t Remove(const AssetHandle handle);
        void Clear();

        auto begin() { return mAssetRegistry.begin(); }
        auto end() { return mAssetRegistry.end(); }
        auto begin() const { return mAssetRegistry.cbegin(); }
        auto end() const { return mAssetRegistry.cend(); }

      private:
        std::unordered_map<AssetHandle, AssetMetaData> mAssetRegistry;
    };

}  // namespace Ethereal
