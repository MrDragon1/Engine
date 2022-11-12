#pragma once
#include "Base/Utils.h"

namespace Ethereal
{

    enum class AssetFlag : uint16_t
    {
        None = 0,
        Missing = BIT(0),
        Invalid = BIT(1)
    };

    enum class AssetType : uint16_t
    {
        None = 0,
        Scene = 1,
        StaticMesh = 2,
        MeshSource = 3,
        Material = 4,
        Texture = 5,
        EnvMap = 6,
        Font = 7,
        Animation = 8,
        Animator = 9,
        Skeleton = 10,
        Mesh = 11
    };

    namespace Utils
    {

        inline AssetType AssetTypeFromString(const std::string& assetType) {
            if (assetType == "None") return AssetType::None;
            if (assetType == "Scene") return AssetType::Scene;
            if (assetType == "StaticMesh") return AssetType::StaticMesh;
            if (assetType == "Mesh") return AssetType::Mesh;
            if (assetType == "MeshSource") return AssetType::MeshSource;
            if (assetType == "Material") return AssetType::Material;
            if (assetType == "Texture") return AssetType::Texture;
            if (assetType == "EnvMap") return AssetType::EnvMap;
            if (assetType == "Font") return AssetType::Font;
            if (assetType == "Animation") return AssetType::Animation;
            if (assetType == "Animator") return AssetType::Animator;
            if (assetType == "Skeleton") return AssetType::Skeleton;

            ET_CORE_ASSERT(false, "Unknown Asset Type");
            return AssetType::None;
        }

        inline const char* AssetTypeToString(AssetType assetType) {
            switch (assetType) {
                case AssetType::None:
                    return "None";
                case AssetType::Scene:
                    return "Scene";
                case AssetType::StaticMesh:
                    return "StaticMesh";
                case AssetType::MeshSource:
                    return "MeshSource";
                case AssetType::Material:
                    return "Material";
                case AssetType::Texture:
                    return "Texture";
                case AssetType::EnvMap:
                    return "EnvMap";
                case AssetType::Font:
                    return "Font";
                case AssetType::Skeleton:
                    return "Skeleton";
                case AssetType::Animation:
                    return "Animation";
                case AssetType::Animator:
                    return "Animator";
                case AssetType::Mesh:
                    return "Mesh";
            }

            ET_CORE_ASSERT(false, "Unknown Asset Type");
            return "None";
        }

    }  // namespace Utils
}  // namespace Ethereal
