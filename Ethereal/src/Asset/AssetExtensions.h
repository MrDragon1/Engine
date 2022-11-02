#pragma once
#include <unordered_map>

#include "AssetTypes.h"

namespace Ethereal
{

    inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap = {{".hscene", AssetType::Scene},
                                                                                    {".hsmesh", AssetType::StaticMesh},
                                                                                    {".hmesh", AssetType::Mesh},
                                                                                    {".hmaterial", AssetType::Material},
                                                                                    {".hAnimation", AssetType::Animation},
                                                                                    {".hAnimator", AssetType::Animator},
                                                                                    {".hSkeleton", AssetType::Skeleton},

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
}
