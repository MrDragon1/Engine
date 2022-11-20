#pragma once

#include "Core/Asset/Asset.h"

namespace Ethereal
{
    REFLECTION_TYPE(EnvironmentDesc)
    CLASS(EnvironmentDesc : public Asset, Fields) {
        REFLECTION_BODY(EnvironmentDesc);

      public:
        virtual AssetType GetAssetType() const override { return AssetType::EnvMap; }

        UUID EnvironmentMap;
    };

}  // namespace Ethereal
