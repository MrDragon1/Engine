#pragma once
#include "Core/Asset/AssetImporter.h"
#include "Core/Asset/AssetRegistry.h"
#include "Core/Project/Project.h"
#include "Utils/FileSystem.h"

#include <map>
#include <unordered_map>
#include <Utils/Hash.h>

namespace Ethereal
{
    class AssetManager {
      public:
        static void Init();
        static void Shutdown();

        static const AssetMetaData& GetMetadata(AssetHandle handle);
        static const AssetMetaData& GetMetadata(const std::filesystem::path& filepath);
        static const AssetMetaData& GetMetadata(const Ref<Asset>& asset) { return GetMetadata(asset->Handle); }

        static std::filesystem::path GetFileSystemPath(const AssetMetaData& metadata) { return Project::GetAssetDirectory() / metadata.FilePath; }
        static std::string GetFileSystemPathString(const AssetMetaData& metadata) { return GetFileSystemPath(metadata).string(); }
        static std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);

        static AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& filepath);
        static bool IsAssetHandleValid(AssetHandle assetHandle) { return IsMemoryAsset(assetHandle) || GetMetadata(assetHandle).IsValid(); }

        static AssetType GetAssetTypeFromExtension(const std::string& extension);
        static AssetType GetAssetTypeFromPath(const std::filesystem::path& path);

        static AssetHandle LoadAsset(const std::filesystem::path& filepath);
        static AssetHandle ImportAsset(const std::filesystem::path& filepath);
        static bool ReloadData(AssetHandle assetHandle);

        template <typename T, typename... Args>
        static Ref<T> CreateNewAsset(const std::string& filename, const std::string& directoryPath, Args&&... args) {
            static_assert(std::is_base_of<Asset, T>::value, "CreateNewAsset only works for types derived from Asset");
            ET_CORE_INFO("Create new asset: {0}", directoryPath + "\\" + filename);
            AssetMetaData metadata;
            metadata.Type = T::GetStaticType();
            metadata.Handle = AssetHandle();

            if (directoryPath.empty() || directoryPath == ".")
                metadata.FilePath = filename;
            else
                metadata.FilePath = AssetManager::GetRelativePath(directoryPath + "/" + filename);
            metadata.IsDataLoaded = true;

            s_AssetRegistry[metadata.Handle] = metadata;

            WriteRegistryToFile();

            Ref<T> asset = Ref<T>::Create(std::forward<Args>(args)...);
            asset->Handle = metadata.Handle;
            s_LoadedAssets[asset->Handle] = asset;
            AssetImporter::Serialize(metadata, asset);

            return asset;
        }

        template <typename T>
        static Ref<T> GetAsset(AssetHandle assetHandle) {
            if (IsMemoryAsset(assetHandle)) return s_MemoryAssets[assetHandle].As<T>();

            auto& metadata = GetMetadataInternal(assetHandle);
            if (!metadata.IsValid()) return nullptr;

            Ref<Asset> asset = nullptr;
            if (!metadata.IsDataLoaded) {
                metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
                if (!metadata.IsDataLoaded) return nullptr;

                s_LoadedAssets[assetHandle] = asset;
            } else {
                asset = s_LoadedAssets[assetHandle];
            }

            return asset.As<T>();
        }

        template <typename T>
        static Ref<T> GetAsset(const std::string& filepath) {
            return GetAsset<T>(GetAssetHandleFromFilePath(filepath));
        }

        static bool FileExists(AssetMetaData& metadata) { return FileSystem::Exists(Project::GetActive()->GetAssetDirectory() / metadata.FilePath); }

        static const std::unordered_map<AssetHandle, Ref<Asset>>& GetLoadedAssets() { return s_LoadedAssets; }
        static const AssetRegistry& GetAssetRegistry() { return s_AssetRegistry; }
        static const std::unordered_map<AssetHandle, Ref<Asset>>& GetMemoryOnlyAssets() { return s_MemoryAssets; }

        static bool IsMemoryAsset(AssetHandle handle) { return s_MemoryAssets.find(handle) != s_MemoryAssets.end(); }

      private:
        static void LoadAssetRegistry();
        static void ProcessDirectory(const std::filesystem::path& directoryPath);
        static void ReloadAssets();
        static void WriteRegistryToFile();
        static void PostProcessAfterImport(const AssetMetaData& metadata);

        static AssetMetaData& GetMetadataInternal(AssetHandle handle);
      private:
        static std::unordered_map<AssetHandle, Ref<Asset>> s_LoadedAssets;
        static std::unordered_map<AssetHandle, Ref<Asset>> s_MemoryAssets;
        inline static AssetRegistry s_AssetRegistry;

      private:
        friend class ContentBrowserPanel;
    };

}  // namespace Ethereal
