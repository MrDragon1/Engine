#pragma once
#include <unordered_map>

#include "AssetTypes.h"

namespace Ethereal
{

    inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap = {{".hscene", AssetType::Scene},
                                                                                    {".hsmesh", AssetType::StaticMesh},
                                                                                    {".hmaterial", AssetType::Material},
                                                                                    {".hanim", AssetType::Animation},
                                                                                    {".hskel", AssetType::Skeleton},

                                                                                    // Meshes
                                                                                    {".fbx", AssetType::MeshSource},
                                                                                    {".gltf", AssetType::MeshSource},
                                                                                    {".glb", AssetType::MeshSource},
                                                                                    {".obj", AssetType::MeshSource},

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
