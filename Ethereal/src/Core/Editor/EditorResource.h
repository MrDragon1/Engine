#pragma once
#include "Core/Renderer/Texture.h"
#include "Resource/ResourceLoader.h"
#include "Utils/FileSystem.h"

#include <filesystem>

namespace Ethereal
{
    class EditorResource {
      public:
        // Generic
        inline static Ref<Texture2D> GearIcon = nullptr;
        inline static Ref<Texture2D> PlusIcon = nullptr;
        inline static Ref<Texture2D> PencilIcon = nullptr;
        inline static Ref<Texture2D> ForwardIcon = nullptr;
        inline static Ref<Texture2D> BackIcon = nullptr;
        inline static Ref<Texture2D> PointerIcon = nullptr;
        inline static Ref<Texture2D> SearchIcon = nullptr;
        inline static Ref<Texture2D> ClearIcon = nullptr;
        inline static Ref<Texture2D> SaveIcon = nullptr;
        inline static Ref<Texture2D> InfoIcon = nullptr;
        inline static Ref<Texture2D> WarningIcon = nullptr;
        inline static Ref<Texture2D> ErrorIcon = nullptr;

        // Icons
        inline static Ref<Texture2D> PointLightIcon = nullptr;
        inline static Ref<Texture2D> CameraIcon = nullptr;
        inline static Ref<Texture2D> AssetIcon = nullptr;

        // Viewport
        inline static Ref<Texture2D> PlayIcon = nullptr;
        inline static Ref<Texture2D> PauseIcon = nullptr;
        inline static Ref<Texture2D> StopIcon = nullptr;
        inline static Ref<Texture2D> SimulateIcon = nullptr;
        inline static Ref<Texture2D> MoveIcon = nullptr;
        inline static Ref<Texture2D> RotateIcon = nullptr;
        inline static Ref<Texture2D> ScaleIcon = nullptr;

        // Window
        inline static Ref<Texture2D> MinimizeIcon = nullptr;
        inline static Ref<Texture2D> MaximizeIcon = nullptr;
        inline static Ref<Texture2D> RestoreIcon = nullptr;
        inline static Ref<Texture2D> CloseIcon = nullptr;

        // Content Browser
        inline static Ref<Texture2D> FolderIcon = nullptr;
        inline static Ref<Texture2D> FileIcon = nullptr;
        inline static Ref<Texture2D> FBXFileIcon = nullptr;
        inline static Ref<Texture2D> OBJFileIcon = nullptr;
        inline static Ref<Texture2D> WAVFileIcon = nullptr;
        // inline static Ref<Texture2D> CSFileIcon = nullptr;
        inline static Ref<Texture2D> PNGFileIcon = nullptr;
        inline static Ref<Texture2D> MaterialFileIcon = nullptr;
        inline static Ref<Texture2D> SceneFileIcon = nullptr;
        inline static Ref<Texture2D> PrefabFileIcon = nullptr;
        inline static Ref<Texture2D> FontFileIcon = nullptr;

        // Node Graph Editor
        inline static Ref<Texture2D> CompileIcon = nullptr;
        inline static Ref<Texture2D> PinValueConnectIcon = nullptr;
        inline static Ref<Texture2D> PinValueDisconnectIcon = nullptr;
        inline static Ref<Texture2D> PinFlowConnectIcon = nullptr;
        inline static Ref<Texture2D> PinFlowDisconnectIcon = nullptr;
        inline static Ref<Texture2D> PinAudioConnectIcon = nullptr;
        inline static Ref<Texture2D> PinAudioDisconnectIcon = nullptr;

        // Textures
        inline static Ref<Texture2D> HazelLogoTexture = nullptr;
        inline static Ref<Texture2D> CheckerboardTexture = nullptr;
        inline static Ref<Texture2D> ShadowTexture = nullptr;
        inline static Ref<Texture2D> TranslucencyTexture = nullptr;

        static void Init() {
            // Generic
            GearIcon = LoadTexture("Generic/Gear.png");
            PlusIcon = LoadTexture("Generic/Plus.png");
            PencilIcon = LoadTexture("Generic/Pencil.png");
            ForwardIcon = LoadTexture("Generic/Forward.png");
            BackIcon = LoadTexture("Generic/Back.png");
            PointerIcon = LoadTexture("Generic/Pointer.png");
            SearchIcon = LoadTexture("Generic/Search.png");
            ClearIcon = LoadTexture("Generic/Clear.png");
            SaveIcon = LoadTexture("Generic/Save.png");
            InfoIcon = LoadTexture("Generic/Info.png");
            WarningIcon = LoadTexture("Generic/Warning.png");
            ErrorIcon = LoadTexture("Generic/Error.png");

            // Icons
            PointLightIcon = LoadTexture("Icons/PointLight.png");
            CameraIcon = LoadTexture("Icons/Camera.png");
            AssetIcon = LoadTexture("Icons/Generic.png");

            // Viewport
            PlayIcon = LoadTexture("Viewport/Play.png");
            PauseIcon = LoadTexture("Viewport/Pause.png");
            StopIcon = LoadTexture("Viewport/Stop.png");
            SimulateIcon = LoadTexture("Viewport/Simulate.png");
            MoveIcon = LoadTexture("Viewport/MoveTool.png");
            RotateIcon = LoadTexture("Viewport/RotateTool.png");
            ScaleIcon = LoadTexture("Viewport/ScaleTool.png");

            // Window
            MinimizeIcon = LoadTexture("Window/Minimize.png");
            MaximizeIcon = LoadTexture("Window/Maximize.png");
            RestoreIcon = LoadTexture("Window/Restore.png");
            CloseIcon = LoadTexture("Window/Close.png");

            // Content Browser
            FolderIcon = LoadTexture("ContentBrowser/Folder.png");
            FileIcon = LoadTexture("ContentBrowser/File.png");
            FBXFileIcon = LoadTexture("ContentBrowser/FBX.png");
            OBJFileIcon = LoadTexture("ContentBrowser/OBJ.png");
            WAVFileIcon = LoadTexture("ContentBrowser/WAV.png");
            // CSFileIcon = LoadTexture("ContentBrowser/csc.png", { "CSFileIcon", TextureWrap::Clamp });
            PNGFileIcon = LoadTexture("ContentBrowser/PNG.png");
            MaterialFileIcon = LoadTexture("ContentBrowser/Material.png");
            SceneFileIcon = LoadTexture("hazel.png");
            PrefabFileIcon = LoadTexture("ContentBrowser/Prefab.png");
            FontFileIcon = LoadTexture("ContentBrowser/Font.png");

            // Node Graph
            CompileIcon = LoadTexture("NodeGraph/Compile.png");
            PinValueConnectIcon = LoadTexture("NodeGraph/Pins/ValueConnect.png");
            PinValueDisconnectIcon = LoadTexture("NodeGraph/Pins/ValueDisconnect.png");
            PinFlowConnectIcon = LoadTexture("NodeGraph/Pins/FlowConnect.png");
            PinFlowDisconnectIcon = LoadTexture("NodeGraph/Pins/FlowDisconnect.png");
            PinAudioConnectIcon = LoadTexture("NodeGraph/Pins/AudioConnect.png");
            PinAudioDisconnectIcon = LoadTexture("NodeGraph/Pins/AudioDisconnect.png");

            // Textures
            HazelLogoTexture = LoadTexture("H_logo.png");
            CheckerboardTexture = LoadTexture("Checkerboard.tga");
            ShadowTexture = LoadTexture("Panels/Shadow.png");
            TranslucencyTexture = LoadTexture("Panels/Translucency.png");
        }

        static void Shutdown() {
            // Must call this function before shutdown

            // Generic
            GearIcon.Reset();
            PlusIcon.Reset();
            PencilIcon.Reset();
            ForwardIcon.Reset();
            BackIcon.Reset();
            PointerIcon.Reset();
            SearchIcon.Reset();
            ClearIcon.Reset();
            SaveIcon.Reset();
            InfoIcon.Reset();
            WarningIcon.Reset();
            ErrorIcon.Reset();

            // Icons
            PointLightIcon.Reset();
            CameraIcon.Reset();
            AssetIcon.Reset();

            // Viewport
            PlayIcon.Reset();
            PauseIcon.Reset();
            StopIcon.Reset();
            SimulateIcon.Reset();
            MoveIcon.Reset();
            RotateIcon.Reset();
            ScaleIcon.Reset();

            // Window
            MinimizeIcon.Reset();
            MaximizeIcon.Reset();
            RestoreIcon.Reset();
            CloseIcon.Reset();

            // Content Browser
            FolderIcon.Reset();
            FileIcon.Reset();
            FBXFileIcon.Reset();
            OBJFileIcon.Reset();
            WAVFileIcon.Reset();
            // CSFileIcon.Reset();
            PNGFileIcon.Reset();
            MaterialFileIcon.Reset();
            SceneFileIcon.Reset();
            PrefabFileIcon.Reset();
            FontFileIcon.Reset();

            // Node Graph
            CompileIcon.Reset();
            PinValueConnectIcon.Reset();
            PinValueDisconnectIcon.Reset();
            PinFlowConnectIcon.Reset();
            PinFlowDisconnectIcon.Reset();
            PinAudioConnectIcon.Reset();
            PinAudioDisconnectIcon.Reset();

            // Textures
            HazelLogoTexture.Reset();
            CheckerboardTexture.Reset();
            ShadowTexture.Reset();
            TranslucencyTexture.Reset();
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
