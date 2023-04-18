#pragma once
#include <filesystem>

#include "Resource/ResourceLoader.h"
#include "Utils/FileSystem.h"

namespace Ethereal {
class EditorResource {
   public:
    inline static Ref<Texture> PlayIcon = nullptr;
    inline static Ref<Texture> StopIcon = nullptr;
    inline static Ref<Texture> AssetIcon = nullptr;
    inline static Ref<Texture> MenuDotsIcon = nullptr;
    inline static Ref<Texture> ExchangeIcon = nullptr;
    inline static Ref<Texture> PlusIcon = nullptr;
    inline static Ref<Texture> MinusIcon = nullptr;
    inline static Ref<Texture> BurgerMenuIcon = nullptr;
    inline static Ref<Texture> EyeIcon = nullptr;
    inline static Ref<Texture> EyeCrossIcon = nullptr;

    // Content Browser Icon
    inline static Ref<Texture> FileIcon = nullptr;
    inline static Ref<Texture> FolderIcon = nullptr;
    inline static Ref<Texture> MeshIcon = nullptr;
    inline static Ref<Texture> StaticMeshIcon = nullptr;
    inline static Ref<Texture> SceneIcon = nullptr;
    inline static Ref<Texture> MissingIcon = nullptr;
    inline static Ref<Texture> UnknownIcon = nullptr;
    inline static Ref<Texture> AnimationIcon = nullptr;
    inline static Ref<Texture> AnimatorIcon = nullptr;
    inline static Ref<Texture> SkeletonIcon = nullptr;

    // Material Graph Editor
    inline static Ref<Texture> MaterialGraphHeaderBG = nullptr;
    inline static Ref<Texture> BlueprintSaveIcon = nullptr;
    inline static Ref<Texture> BlueprintRestoreIcon = nullptr;

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

        // Content Browser Icon
        FolderIcon = LoadTexture("ContentBrowser/folder.png");
        MissingIcon = LoadTexture("ContentBrowser/missing.png");
        UnknownIcon = LoadTexture("ContentBrowser/unknown.png");
        FileIcon = LoadTexture("ContentBrowser/file.png");
        MeshIcon = LoadTexture("ContentBrowser/mesh.png");
        StaticMeshIcon = LoadTexture("ContentBrowser/staticmesh.png");
        SceneIcon = LoadTexture("ContentBrowser/scene.png");
        AnimationIcon = LoadTexture("ContentBrowser/animation.png");
        AnimatorIcon = LoadTexture("ContentBrowser/animator.png");
        SkeletonIcon = LoadTexture("ContentBrowser/skeleton.png");

        // Material Graph Editor
        MaterialGraphHeaderBG = LoadTexture("BlueprintBackground.png");
        BlueprintSaveIcon = LoadTexture("BlueprintSave.png");
        BlueprintRestoreIcon = LoadTexture("BlueprintRestore.png");
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

        // Content Browser Icon
        FolderIcon.Reset();
        FileIcon.Reset();
        MeshIcon.Reset();
        StaticMeshIcon.Reset();
        SceneIcon.Reset();
        MissingIcon.Reset();
        UnknownIcon.Reset();
        AnimationIcon.Reset();
        AnimatorIcon.Reset();
        SkeletonIcon.Reset();

        // Material Graph Editor
        MaterialGraphHeaderBG.Reset();
        BlueprintSaveIcon.Reset();
        BlueprintRestoreIcon.Reset();
    }

   private:
    static Ref<Texture> LoadTexture(const std::filesystem::path& relativePath) {
        std::filesystem::path path = std::filesystem::path("assets") / "buildin" / "Editor" / relativePath;

        if (!FileSystem::Exists(path)) {
            ET_CORE_ASSERT(false, "Failed to load icon {0}! The file doesn't exist.", path.string());
            return nullptr;
        }
        return ResourceLoader::LoadTexture(path.string());
    }
};
}  // namespace Ethereal
