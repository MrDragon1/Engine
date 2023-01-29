#pragma once

#include "Core/Asset/Asset.h"

namespace Ethereal
{

    REFLECTION_TYPE(MaterialTableRaw)
    CLASS(MaterialTableRaw, Fields) {
        REFLECTION_BODY(MaterialTableRaw);
      public:
        std::vector<UUID> Materials = {-1};
    };

    REFLECTION_TYPE(MaterialDesc)
    CLASS(MaterialDesc : public Asset, Fields) {
        REFLECTION_BODY(MaterialDesc);

      public:
        MaterialDesc() = default;
        MaterialDesc(const MaterialDesc& other) {
            this->Name = other.Name;
            this->AlbedoMap = other.AlbedoMap;
            this->NormalMap = other.NormalMap;
            this->MetallicMap = other.MetallicMap;
            this->RoughnessMap = other.RoughnessMap;
            this->OcclusionMap = other.OcclusionMap;
            this->Albedo = other.Albedo;
            this->Metallic = other.Metallic;
            this->Roughness = other.Roughness;
            this->Transparency = other.Transparency;

            this->IsAlbedo = other.IsAlbedo;
            this->IsMetallic = other.IsMetallic;
            this->IsRoughness = other.IsRoughness;
            this->IsNormal = other.IsNormal;
            this->IsOcclusion = other.IsOcclusion;
            this->IsTransparent = other.IsTransparent;
        }
        static AssetType GetStaticType() { return AssetType::Material; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }
      public:
        UUID AlbedoMap;
        UUID NormalMap;
        UUID MetallicMap;
        UUID RoughnessMap;
        UUID OcclusionMap;

        Vector3 Albedo = Vector3(1.0f);
        float Metallic = 0.0f;
        float Roughness = 1.0f;
        float Emisstion = 0.0f;
        float Transparency = 0.0f;

        bool IsAlbedo = false;
        bool IsMetallic = false;
        bool IsRoughness = false;
        bool IsNormal = false;
        bool IsOcclusion = false;

        bool IsTransparent = false;

        std::string Name;
    };
}