#pragma once

#include "Core/Base/Singleton.h"
#include <Renderer/Texture.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Ethereal
{
    //    class ObjLoader : public Singleton<ObjLoader> {
    //      public:
    //        static void Load(const GameObjectMeshDesc& desc, RenderMeshData& renderMeshData);
    //
    //      private:
    //        static std::vector<MeshVertex> m_MeshVertices;
    //        static std::vector<unsigned int> m_MeshIndices;
    //
    //      private:
    //        static void processNode(aiNode* node, const aiScene* scene);
    //        static void processMesh(aiMesh* mesh, const aiScene* scene);
    //    };

    class TextureLoader : public Singleton<TextureLoader> {
      public:
        static void LoadPath(const std::string& path, Ref<TextureData>& textureData, bool flip = true);
    };
}  // namespace Ethereal
