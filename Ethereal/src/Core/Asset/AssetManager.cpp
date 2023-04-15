#include "Core/Asset/AssetManager.h"

#include <filesystem>
#include <fstream>

#include "Utils/Yaml.h"
#include "Core/Asset/AssetExtensions.h"
#include "Core/Renderer/Mesh.h"
#include "Utils/StringUtils.h"
#include "imgui.h"
#include "pch.h"

namespace Ethereal {

static AssetMetaData sNullMetadata;

void AssetManager::Init() {
    LoadAssetRegistry();
    ReloadAssets();
}

void AssetManager::Shutdown() {
    WriteRegistryToFile();

    sMemoryAssets.clear();
    sAssetRegistry.Clear();
    sLoadedAssets.clear();
}

AssetMetaData& AssetManager::GetMetadataInternal(AssetHandle handle) {
    if (sAssetRegistry.Contains(handle)) return sAssetRegistry[handle];

    return sNullMetadata;  // make sure to check return value before you go changing it!
}

const AssetMetaData& AssetManager::GetMetadata(AssetHandle handle) {
    if (sAssetRegistry.Contains(handle)) return sAssetRegistry[handle];

    return sNullMetadata;
}

const AssetMetaData& AssetManager::GetMetadata(const std::filesystem::path& filepath) {
    const auto relativePath = GetRelativePath(filepath);

    for (auto& [handle, metadata] : sAssetRegistry) {
        if (metadata.FilePath == relativePath) return metadata;
    }

    return sNullMetadata;
}

std::filesystem::path AssetManager::GetRelativePath(const std::filesystem::path& filepath) {
    std::filesystem::path relativePath = filepath.lexically_normal();
    std::string temp = filepath.string();
    if (temp.find(Project::GetAssetDirectory().string()) != std::string::npos) {
        relativePath = std::filesystem::relative(filepath, Project::GetAssetDirectory());
        if (relativePath.empty()) {
            relativePath = filepath.lexically_normal();
        }
    }
    return relativePath;
}

AssetHandle AssetManager::GetAssetHandleFromFilePath(const std::filesystem::path& filepath) { return GetMetadata(filepath).Handle; }

AssetType AssetManager::GetAssetTypeFromExtension(const std::string& extension) {
    std::string ext = extension;
    //        std::string ext = Utils::String::ToLowerCopy(extension);
    if (sAssetExtensionMap.find(ext) == sAssetExtensionMap.end()) return AssetType::None;

    return sAssetExtensionMap.at(ext.c_str());
}

AssetType AssetManager::GetAssetTypeFromPath(const std::filesystem::path& path) { return GetAssetTypeFromExtension(path.extension().string()); }

Ref<Texture> AssetManager::GetAssetIconFromExtension(const std::string& extension) { return GetAssetIcon(GetAssetTypeFromExtension(extension)); }

void AssetManager::LoadAssetRegistry() {
    // TODO:
    //  BUG: Same asset may has many handle in registry file.
    //  Importing asset annually will add a new handle for all the associated asset file.
    ET_CORE_INFO("[AssetManager] Loading Asset Registry");

    const auto& assetRegistryPath = Project::GetAssetRegistryPath();
    if (!FileSystem::Exists(assetRegistryPath)) return;

    std::ifstream stream(assetRegistryPath);
    ET_CORE_ASSERT(stream);
    std::stringstream strStream;
    strStream << stream.rdbuf();

    YNode data = YAML::Load(strStream.str());
    auto handles = data["Assets"];
    if (!handles) {
        ET_CORE_ERROR("[AssetManager] Asset Registry appears to be corrupted!");
        //            ET_CORE_VERIFY(false);
        return;
    }

    for (auto entry : handles) {
        std::string filepath = entry["FilePath"].as<std::string>();

        AssetMetaData metadata;
        metadata.Handle = entry["Handle"].as<uint64_t>();
        metadata.FilePath = filepath;
        metadata.Type = (AssetType)Utils::AssetTypeFromString(entry["Type"].as<std::string>());

        if (metadata.Type == AssetType::None) continue;

        if (!FileSystem::Exists(AssetManager::GetFileSystemPath(metadata))) {
            ET_CORE_WARN("[AssetManager] Missing asset '{0}' detected in registry file", metadata.FilePath.string());
            continue;
        }

        if (metadata.Handle == 0) {
            ET_CORE_WARN("[AssetManager] AssetHandle for {0} is 0, this shouldn't happen.", metadata.FilePath.string());
            continue;
        }

        sAssetRegistry[metadata.Handle] = metadata;
    }

    ET_CORE_INFO("[AssetManager] Loaded {0} asset entries", sAssetRegistry.Count());
}

AssetHandle AssetManager::LoadAsset(const std::filesystem::path& filepath) {
    std::filesystem::path path = GetRelativePath(filepath);

    if (auto& metadata = GetMetadata(path); metadata.IsValid()) return metadata.Handle;

    AssetType type = GetAssetTypeFromPath(path);
    if (type == AssetType::None) return 0;

    AssetMetaData metadata;
    metadata.Handle = AssetHandle();
    metadata.FilePath = path;
    metadata.Type = type;
    sAssetRegistry[metadata.Handle] = metadata;

    return metadata.Handle;
}

bool AssetManager::ReloadData(AssetHandle assetHandle) {
    auto& metadata = GetMetadataInternal(assetHandle);
    if (!metadata.IsValid()) {
        ET_CORE_ERROR("Trying to reload invalid asset");
        return false;
    }

    if (!metadata.IsDataLoaded) {
        ET_CORE_WARN("Trying to reload asset that was never loaded");

        Ref<Asset> asset;
        metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
        return metadata.IsDataLoaded;
    }

    ET_CORE_ASSERT(sLoadedAssets.find(assetHandle) != sLoadedAssets.end());
    Ref<Asset>& asset = sLoadedAssets.at(assetHandle);
    metadata.IsDataLoaded = AssetImporter::TryLoadData(metadata, asset);
    return metadata.IsDataLoaded;
}

void AssetManager::ProcessDirectory(const std::filesystem::path& directoryPath) {
    for (auto entry : std::filesystem::directory_iterator(directoryPath)) {
        if (entry.is_directory())
            ProcessDirectory(entry.path());
        else
            LoadAsset(entry.path());
    }
}

void AssetManager::ReloadAssets() {
    ProcessDirectory(Project::GetAssetDirectory().string());
    WriteRegistryToFile();
}

void AssetManager::WriteRegistryToFile() {
    // Sort assets by UUID to make project managment easier
    struct AssetRegistryEntry {
        std::string FilePath;
        AssetType Type;
    };
    std::map<UUID, AssetRegistryEntry> sortedMap;
    for (auto& [filepath, metadata] : sAssetRegistry) {
        if (!FileSystem::Exists(GetFileSystemPath(metadata))) continue;

        if (metadata.IsMemoryAsset) continue;

        std::string pathToSerialize = metadata.FilePath.lexically_normal().string();
        sortedMap[metadata.Handle] = {pathToSerialize, metadata.Type};
    }

    ET_CORE_INFO("[AssetManager] serializing asset registry with {0} entries", sortedMap.size());

    YAML::Emitter out;
    out << YAML::BeginMap;

    out << YAML::Key << "Assets" << YAML::BeginSeq;
    for (auto& [handle, entry] : sortedMap) {
        out << YAML::BeginMap;
        out << YAML::Key << "Handle" << YAML::Value << handle;
        out << YAML::Key << "FilePath" << YAML::Value << entry.FilePath;
        out << YAML::Key << "Type" << YAML::Value << Utils::AssetTypeToString(entry.Type);
        out << YAML::EndMap;
    }
    out << YAML::EndSeq;
    out << YAML::EndMap;

    FileSystem::SkipNextFileSystemChange();

    const std::string& assetRegistryPath = Project::GetAssetRegistryPath().string();
    std::ofstream fout(assetRegistryPath);
    fout << out.c_str();
}

std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::sLoadedAssets;
std::unordered_map<AssetHandle, Ref<Asset>> AssetManager::sMemoryAssets;
}  // namespace Ethereal
