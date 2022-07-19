#pragma once

// #define TINYOBJLOADER_IMPLEMENTATION  // define this in only *one* .cc
// Optional. define TINYOBJLOADER_USE_MAPBOX_EARCUT gives robust trinagulation. Requires C++11
//#define TINYOBJLOADER_USE_MAPBOX_EARCUT
#include "tiny_obj_loader.h"

#include "Ethereal/Renderer/Common.hpp"
namespace Ethereal
{

    class ObjLoader {
      public:
        static void Load(const GameObjectMeshDesc& desc, RenderMeshData& renderMeshData, int entityID = -1);
    };

    class TextureLoader {
      public:
        static void LoadPath(const std::string& path, Ref<TextureData>& textureData, bool flip = true);
    };
}  // namespace Ethereal
