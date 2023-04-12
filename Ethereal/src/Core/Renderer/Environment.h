#pragma once
#include "Platform/Windows/Backend/Driver.h"
namespace Ethereal {
class Environment : public Asset {
   public:
    Ref<Texture> RadianceMap;
    Ref<Texture> IrradianceMap;

    Environment() = default;
    Environment(const Ref<Texture>& radianceMap, const Ref<Texture>& irradianceMap) : RadianceMap(radianceMap), IrradianceMap(irradianceMap) {}

    static AssetType GetStaticType() { return AssetType::EnvMap; }
    virtual AssetType GetAssetType() const override { return GetStaticType(); }
};

}  // namespace Ethereal
