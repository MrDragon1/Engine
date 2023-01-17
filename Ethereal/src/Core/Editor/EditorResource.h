#pragma once
#include "Core/Renderer/Texture.h"
#include "Resource/ResourceLoader.h"
#include "Utils/FileSystem.h"

#include <filesystem>

namespace Ethereal
{
    class EditorResource {
      public:

        inline static Ref<Texture2D> PlayIcon = nullptr;
        inline static Ref<Texture2D> StopIcon = nullptr;
        inline static Ref<Texture2D> AssetIcon = nullptr;
        inline static Ref<Texture2D> MenuDotsIcon = nullptr;

        static void Init() {
            PlayIcon = LoadTexture("play.png");
            StopIcon = LoadTexture("stop.png");
            MenuDotsIcon = LoadTexture("menu-dots.png");


            AssetIcon = LoadTexture("cube.png");
        }

        static void Shutdown() {
            // Must call this function before shutdown
            PlayIcon.Reset();
            StopIcon.Reset();
            MenuDotsIcon.Reset();
            AssetIcon.Reset();
        }

      private:
        static Ref<Texture2D> LoadTexture(const std::filesystem::path& relativePath) {
            std::filesystem::path path = std::filesystem::path("assets") /"buildin" / "Editor" / relativePath;

            if (!FileSystem::Exists(path)) {
                ET_CORE_ASSERT(false,"Failed to load icon {0}! The file doesn't exist.", path.string());
                return nullptr;
            }

            return Texture2D::Create(path.string());
        }
    };
}  // namespace Ethereal
