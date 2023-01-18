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
        inline static Ref<Texture2D> ExchangeIcon = nullptr;
        inline static Ref<Texture2D> PlusIcon = nullptr;
        inline static Ref<Texture2D> MinusIcon = nullptr;
        inline static Ref<Texture2D> BurgerMenuIcon = nullptr;
        inline static Ref<Texture2D> EyeIcon = nullptr;
        inline static Ref<Texture2D> EyeCrossIcon = nullptr;

        //Content Browser Icon
        inline static Ref<Texture2D> FileIcon = nullptr;
        inline static Ref<Texture2D> FolderIcon = nullptr;
        inline static Ref<Texture2D> MeshIcon = nullptr;
        inline static Ref<Texture2D> StaticMeshIcon = nullptr;
        inline static Ref<Texture2D> SceneIcon = nullptr;
        inline static Ref<Texture2D> MissingIcon = nullptr;
        inline static Ref<Texture2D> UnknownIcon = nullptr;



        static void Init() {
            PlayIcon = LoadTexture("play.png");
            StopIcon = LoadTexture("stop.png");
            MenuDotsIcon = LoadTexture("menu-dots.png");
            ExchangeIcon = LoadTexture("exchange.png");

            PlusIcon = LoadTexture("plus.png");
            MinusIcon = LoadTexture("minus.png");

            BurgerMenuIcon = LoadTexture("burger-menu.png");

            AssetIcon = LoadTexture("cube.png");

            EyeIcon = LoadTexture("eye.png");
            EyeCrossIcon = LoadTexture("eye-crossed.png");

            //Content Browser Icon
            FolderIcon = LoadTexture("ContentBrowser/folder.png");
            MissingIcon = LoadTexture("ContentBrowser/missing.png");
            UnknownIcon = LoadTexture("ContentBrowser/unknown.png");
            FileIcon = LoadTexture("ContentBrowser/file.png");
            MeshIcon = LoadTexture("ContentBrowser/mesh.png");
            StaticMeshIcon = LoadTexture("ContentBrowser/staticmesh.png");
            SceneIcon = LoadTexture("ContentBrowser/scene.png");
        }

        static void Shutdown() {
            // Must call this function before shutdown
            PlayIcon.Reset();
            StopIcon.Reset();
            MenuDotsIcon.Reset();
            ExchangeIcon.Reset();
            AssetIcon.Reset();

            PlusIcon.Reset();
            MinusIcon.Reset();
            BurgerMenuIcon.Reset();

            EyeIcon.Reset();
            EyeCrossIcon.Reset();

            //Content Browser Icon
            FolderIcon.Reset();
            FileIcon.Reset();
            MeshIcon.Reset();
            StaticMeshIcon.Reset();
            SceneIcon.Reset();
            MissingIcon.Reset();
            UnknownIcon.Reset();
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
