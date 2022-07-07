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
        static void Load(const GameObjectMeshDesc& desc, Ref<BufferData>& VertexBuffer, Ref<BufferData>& IndexBuffer);
    };
}  // namespace Ethereal
