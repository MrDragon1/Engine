#pragma once
#include "Base/UUID.h"
#include "Base/Ref.h"

#include "Core/Asset/AssetTypes.h"

namespace Ethereal
{
    using AssetHandle = UUID;

    class Asset : public RefCounted {
      public:
        AssetHandle Handle = 0;
        uint16_t Flags = (uint16_t)AssetFlag::None;

        virtual ~Asset() {}

        static AssetType GetStaticType() { return AssetType::None; }
        virtual AssetType GetAssetType() const { return AssetType::None; }

        bool IsValid() const { return ((Flags & (uint16_t)AssetFlag::Missing) | (Flags & (uint16_t)AssetFlag::Invalid)) == 0; }
        bool IsCreated() const { return Handle; }

        virtual bool operator==(const Asset& other) const { return Handle == other.Handle; }

        virtual bool operator!=(const Asset& other) const { return !(*this == other); }

        bool IsFlagSet(AssetFlag flag) const { return (uint16_t)flag & Flags; }
        void SetFlag(AssetFlag flag, bool value = true) {
            if (value)
                Flags |= (uint16_t)flag;
            else
                Flags &= ~(uint16_t)flag;
        }
    };
}  // namespace Ethereal