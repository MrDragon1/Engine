#pragma once
#include "Core/Asset/AssetMetaData.h"

#include <unordered_map>

namespace Ethereal
{
    class AssetRegistry {
      public:
        AssetMetaData& operator[](const AssetHandle handle);
        const AssetMetaData& Get(const AssetHandle handle) const;

        size_t Count() const { return m_AssetRegistry.size(); }
        bool Contains(const AssetHandle handle) const;
        size_t Remove(const AssetHandle handle);
        void Clear();

        auto begin() { return m_AssetRegistry.begin(); }
        auto end() { return m_AssetRegistry.end(); }
        auto begin() const { return m_AssetRegistry.cbegin(); }
        auto end() const { return m_AssetRegistry.cend(); }

      private:
        std::unordered_map<AssetHandle, AssetMetaData> m_AssetRegistry;
    };

}  // namespace Ethereal
