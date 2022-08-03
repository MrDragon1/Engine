#pragma once

#pragma once

#include <unordered_map>

#include "AssetTypes.h"

namespace Ethereal
{

    inline static std::unordered_map<std::string, AssetType> s_AssetExtensionMap = {
        // Hazel types
        {".hscene", AssetType::Scene},
        {".hmesh", AssetType::Mesh},
        {".hsmesh", AssetType::StaticMesh},
        {".hmaterial", AssetType::Material},
        {".hprefab", AssetType::Prefab},
        {".hsoundc", AssetType::SoundConfig},
        {".hpm", AssetType::PhysicsMat},

        // Meshes
        {".fbx", AssetType::MeshSource},
        {".gltf", AssetType::MeshSource},
        {".glb", AssetType::MeshSource},
        {".obj", AssetType::MeshSource},

        // Textures
        {".png", AssetType::Texture},
        {".jpg", AssetType::Texture},
        {".jpeg", AssetType::Texture},
        {".hdr", AssetType::EnvMap},

        // Audio
        {".wav", AssetType::Audio},
        {".ogg", AssetType::Audio},

        // Fonts
        {".ttf", AssetType::Font},
        {".ttc", AssetType::Font},
        {".otf", AssetType::Font},

        // Mesh Collider
        {".hmc", AssetType::MeshCollider},

        // Graphs
        {".sound_graph", AssetType::SoundGraphSound}};

}
