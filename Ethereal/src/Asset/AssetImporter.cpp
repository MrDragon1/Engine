#include "pch.h"
#include "AssetImporter.h"
#include "AssetManager.h"
#include "MeshSerializer.h"

namespace Ethereal
{
    void AssetImporter::Init() {
        s_Serializers[AssetType::Texture] = CreateScope<TextureSerializer>();
        s_Serializers[AssetType::Scene] = CreateScope<SceneAssetSerializer>();
        s_Serializers[AssetType::StaticMesh] = CreateScope<StaticMeshSerializer>();
        s_Serializers[AssetType::MeshSource] = CreateScope<MeshAssetSerializer>();
        s_Serializers[AssetType::Material] = CreateScope<MaterialAssetSerializer>();
        s_Serializers[AssetType::EnvMap] = CreateScope<EnvironmentSerializer>();
    }

    void AssetImporter::Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) {
        if (s_Serializers.find(metadata.Type) == s_Serializers.end()) {
            ET_CORE_WARN("There's currently no importer for assets of type {0}", metadata.FilePath.stem().string());
            return;
        }

        s_Serializers[asset->GetAssetType()]->Serialize(metadata, asset);
    }

    void AssetImporter::Serialize(const Ref<Asset>& asset) {
        const AssetMetaData& metadata = AssetManager::GetMetadata(asset->Handle);
        Serialize(metadata, asset);
    }

    bool AssetImporter::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) {
        if (s_Serializers.find(metadata.Type) == s_Serializers.end()) {
            ET_CORE_WARN("There's currently no importer for assets of type {0}", metadata.FilePath.stem().string());
            return false;
        }

        return s_Serializers[metadata.Type]->TryLoadData(metadata, asset);
    }

    std::unordered_map<AssetType, Scope<AssetSerializer>> AssetImporter::s_Serializers;

}  // namespace Ethereal