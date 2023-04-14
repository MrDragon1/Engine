#pragma once
#include <filesystem>

#include "Utils/EngineMarco.h"
#include "Core/Project/ConfigManager.h"

namespace Ethereal {
struct ProjectConfig {
    std::string Name;

    std::string AssetDirectory = "assets";
    std::string AssetRegistryPath = "assets\\AssetRegistry.etr";

    std::string MeshPath = "assets\\meshes";
    std::string MeshSourcePath = "assets\\meshes\\source";

    std::string StartScene;

    bool ReloadAssemblyOnPlay;

    bool EnableAutoSave = false;
    int AutoSaveIntervalSeconds = 300;

    // Not serialized
    std::string ProjectFileName = "Default Project";
    std::string ProjectDirectory = "E:\\Code\\Engine\\Editor";
};

class Project : public RefCounted {
   public:
    Project();
    ~Project();

    static const ProjectConfig& GetConfig() { return m_Config; }

    static Ref<Project> GetActive() { return s_ActiveProject; }
    static void SetActive(Ref<Project> project);
    static const std::string& GetProjectName() {
        ET_CORE_ASSERT(s_ActiveProject);
        return s_ActiveProject->GetConfig().Name;
    }

    static std::filesystem::path GetProjectDirectory() {
        ET_CORE_ASSERT(s_ActiveProject);
        return s_ActiveProject->GetConfig().ProjectDirectory;
    }

    static std::filesystem::path GetAssetDirectory() {
        ET_CORE_ASSERT(s_ActiveProject);
        return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetDirectory;
    }

    static std::filesystem::path GetAssetRegistryPath() {
        ET_CORE_ASSERT(s_ActiveProject);
        return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().AssetRegistryPath;
    }

    static std::filesystem::path GetMeshPath() {
        ET_CORE_ASSERT(s_ActiveProject);
        return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / s_ActiveProject->GetConfig().MeshPath;
    }

    static std::filesystem::path GetCacheDirectory() {
        ET_CORE_ASSERT(s_ActiveProject);
        return std::filesystem::path(s_ActiveProject->GetConfig().ProjectDirectory) / "Cache";
    }

    static const ConfigManager& GetConfigManager() { return sConfigManager; };

   private:
    void OnDeserialized();

   private:
    // TODO: merge projectconfig & projectsetting to configmanager
    inline static ProjectConfig m_Config;
    inline static Ref<Project> s_ActiveProject;
    inline static ConfigManager sConfigManager;
};

}  // namespace Ethereal
