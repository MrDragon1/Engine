#pragma once
#include "AssetMetaData.h"

namespace Ethereal
{
    class AssetSerializer {
      public:
        virtual void Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const = 0;
        virtual bool TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const = 0;
    };

    class TextureSerializer : public AssetSerializer {
      public:
        virtual void Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const override {}
        virtual bool TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const override;
    };

    class MeshAssetSerializer : public AssetSerializer {
      public:
        virtual void Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const override {}
        virtual bool TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const override;
    };

    class MaterialAssetSerializer : public AssetSerializer {
      public:
        virtual void Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const override;
        virtual bool TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const override;
    };

}  // namespace Ethereal
