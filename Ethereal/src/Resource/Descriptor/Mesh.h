#pragma once

#include "Core/Asset/Asset.h"

namespace Ethereal
{
    REFLECTION_TYPE(StaticMeshDesc)
    CLASS(StaticMeshDesc : public Asset, Fields)
    {
        REFLECTION_BODY(StaticMeshDesc);
      public:
        virtual AssetType GetAssetType() const override { return AssetType::StaticMesh; }
      public:
        UUID Mesh;
        std::vector<UUID> Materials;
    };

    REFLECTION_TYPE(MeshDesc)
    CLASS(MeshDesc : public Asset, Fields) {
        REFLECTION_BODY(MeshDesc);
      public:
        virtual AssetType GetAssetType() const override { return AssetType::Mesh; }

      public:
        UUID Mesh;
        std::vector<UUID> Materials;
        UUID Animator;
    };


}  // namespace Ethereal
