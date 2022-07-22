#pragma once

#include "Ethereal/Core/Base/Singleton.hpp"
#include <filesystem>
#include <glm/glm.hpp>

namespace Ethereal
{
    class ConfigManager final : public Singleton<ConfigManager> {
      public:
        ConfigManager();
        ConfigManager(const ConfigManager&) = delete;
        ConfigManager& operator=(const ConfigManager&) = delete;

      public:
        void Clear();

        static const std::filesystem::path& GetRootFolder() ;
        static const std::filesystem::path& GetAssetsFolder() ;
        static const std::filesystem::path& GetShadersFolder() ;
        static const std::filesystem::path& GetResourcesFolder() ;

      private:
        static std::filesystem::path mRootFolder;
        static std::filesystem::path mAssetsFolder;
        static std::filesystem::path mShadersFolder;
        static std::filesystem::path mResourcesFolder;
    };
}  // namespace Ethereal