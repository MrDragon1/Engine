#pragma once
#include <unordered_map>
#include <Core/Editor/EditorResource.h>

#include "Core/Asset/AssetTypes.h"

namespace Ethereal
{

    inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap = {{".EScene", AssetType::Scene},
                                                                                    {".ESMesh", AssetType::StaticMesh},
                                                                                    {".EMesh", AssetType::Mesh},
                                                                                    {".EMaterial", AssetType::Material},
                                                                                    {".EAnimation", AssetType::Animation},
                                                                                    {".EAnimator", AssetType::Animator},
                                                                                    {".ESkeleton", AssetType::Skeleton},

                                                                                    // Meshes
                                                                                    {".fbx", AssetType::MeshSource},
                                                                                    {".gltf", AssetType::MeshSource},
                                                                                    {".glb", AssetType::MeshSource},
                                                                                    {".obj", AssetType::MeshSource},
                                                                                    {".dae", AssetType::MeshSource},

                                                                                    // Textures
                                                                                    {".png", AssetType::Texture},
                                                                                    {".jpg", AssetType::Texture},
                                                                                    {".jpeg", AssetType::Texture},
                                                                                    {".tga", AssetType::Texture},
                                                                                    {".hdr", AssetType::EnvMap},

                                                                                    // Fonts
                                                                                    {".ttf", AssetType::Font},
                                                                                    {".ttc", AssetType::Font},
                                                                                    {".otf", AssetType::Font}};
    inline static Ref<Texture2D> GetAssetIcon(AssetType type) {
        switch (type) {
            case AssetType::Scene: return EditorResource::SceneIcon;
            case AssetType::StaticMesh: return EditorResource::StaticMeshIcon;
            case AssetType::Material: return EditorResource::FileIcon;
            case AssetType::Animation: return EditorResource::AnimationIcon;
            case AssetType::Animator: return EditorResource::AnimatorIcon;
            case AssetType::Skeleton: return EditorResource::SkeletonIcon;
            case AssetType::Mesh: return EditorResource::MeshIcon;
            default: {
                //ET_CORE_WARN("Unknown asset type, can not get AssetIcon!");
                return EditorResource::UnknownIcon;
            }
        }
    }


    inline static std::string GetAssetSuffix(AssetType type) {
        switch (type) {
            case AssetType::Scene: return ".EScene";
            case AssetType::StaticMesh: return ".ESMesh";
            case AssetType::Material: return ".EMaterial";
            case AssetType::Animation: return ".EAnimation";
            case AssetType::Animator: return ".EAnimator";
            case AssetType::Skeleton: return ".ESkeleton";
            case AssetType::Mesh: return ".EMesh";
            default: {
                ET_CORE_ASSERT(false, "Unknown asset type, can not get suffix!");
                return "";
            }
        }
    }
}
