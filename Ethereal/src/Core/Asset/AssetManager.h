#pragma once
#include "Core/Asset/AssetImporter.h"
#include "Core/Asset/AssetRegistry.h"
#include "Core/Project/Project.h"
#include "Utils/FileSystem.h"
#include "Base/Meta/Serializer.h"
#include "Base/Meta/_generated/serializer/all_serializer.h"
#include "AssetExtensions.h"
#include "Base/Math/Hash.h"

#include <map>
#include <unordered_map>

namespace Ethereal
{
    class AssetManager {
      public:
        static void Init();
        static void Shutdown();

        static const AssetMetaData& GetMetadata(AssetHandle handle);
        static const AssetMetaData& GetMetadata(const std::filesystem::path& filepath);
        static const AssetMetaData& GetMetadata(const Ref<Asset>& asset) { return GetMetadata(asset->Handle); }

        static const std::string GetDisplayTypeName(AssetHandle handle) {
            return Utils::AssetTypeToString(GetMetadata(handle).Type);
        }

        static const std::string GetAssetName(AssetHandle handle) {
            return AssetManager::GetMetadata(handle).FilePath.stem().string();
        }

        static std::filesystem::path GetFileSystemPath(const AssetMetaData& metadata) { return Project::GetAssetDirectory() / metadata.FilePath; }
        static std::string GetFileSystemPathString(const AssetMetaData& metadata) { return GetFileSystemPath(metadata).string(); }
        static std::filesystem::path GetRelativePath(const std::filesystem::path& filepath);

        static AssetHandle GetAssetHandleFromFilePath(const std::filesystem::path& filepath);
        static bool IsAssetHandleValid(AssetHandle assetHandle) { return IsMemoryAsset(assetHandle) || GetMetadata(assetHandle).IsValid(); }

        static AssetType GetAssetTypeFromExtension(const std::string& extension);
        static AssetType GetAssetTypeFromPath(const std::filesystem::path& path);

        static Ref<Texture2D> GetAssetIconFromExtension(const std::string& extension);

        template <typename T>
        static bool LoadAsset_Ref(const std::filesystem::path& filepath, T& asset) {
            YNode data;
            try {
                data = YAML::LoadFile(filepath.string());
            } catch (YAML::ParserException e) {
                ET_CORE_ERROR("Failed to load file '{0}'\n     {1}", filepath.string(), e.what());
                return false;
            }
            Serializer::read(data, asset);
            return true;
        }

        template <typename T>
        static bool SaveAsset_Ref(const std::filesystem::path& filepath, const T& asset) {
            YNode&& node = Serializer::write(asset);
            std::ofstream fout(filepath.string());
            if (!fout)
            {
                ET_CORE_ERROR("open file {} failed!", filepath.string());
                return false;
            }
            YAML::Emitter out;
            out << node;
            fout << out.c_str();
            return true;
        }

        template <typename T>
        static bool CreateAsset_Ref(const std::string& filename, const std::string& directoryPath, T& asset) {
            static_assert(std::is_base_of<Asset, T>::value, "CreateNewAsset only works for types derived from Asset");
            AssetMetaData metadata;
            metadata.Type = T::GetStaticType();
            metadata.Handle = AssetHandle();

            std::string suffix = GetAssetSuffix(metadata.Type);
            auto path = directoryPath + "\\" + filename + suffix;
            ET_CORE_INFO("Create new asset: {0}", path);
            std::ofstream fout(path);
            if (!fout)
            {
                ET_CORE_ERROR("open file {} failed!", path);
                return false;
            }
            if (directoryPath.empty() || directoryPath == ".")
                metadata.FilePath = filename + suffix;
            else
                metadata.FilePath = AssetManager::GetRelativePath(directoryPath + "/" + filename + suffix);

            //TODO: actually the metadata is not loaded,
            // only the descriptor is stored in the s_LoadedAssets.
            // Need to store the actual asset data (not descriptor data).
            // For now, just set the IsDataLoaded to false
            metadata.IsDataLoaded = false;

            s_AssetRegistry[metadata.Handle] = metadata;

            WriteRegistryToFile();

            asset.Handle = metadata.Handle;
//            s_LoadedAssets[asset.Handle] = &asset;

            YNode&& node = Serializer::write(asset);
            YAML::Emitter out;
            out << node;
            fout << out.c_str();

            ReloadAssets();
            return true;
        }

        static AssetHandle LoadAsset(const std::filesystem::path& filepath);
        static bool ReloadData(AssetHandle assetHandle);

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

        static AssetMetaData& GetMetadataInternal(AssetHandle handle);
      private:
        static std::unordered_map<AssetHandle, Ref<Asset>> s_LoadedAssets;
        static std::unordered_map<AssetHandle, Ref<Asset>> s_MemoryAssets;
        inline static AssetRegistry s_AssetRegistry;

      private:
        friend class ContentBrowserPanel;
    };

}  // namespace Ethereal
