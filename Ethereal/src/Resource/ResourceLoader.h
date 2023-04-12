#pragma once

#include "Core/Renderer/Environment.h"
#include "Core/Renderer/Mesh.h"

namespace Ethereal {

class ResourceLoader {
   public:
    static Ref<Texture> LoadTexture(const std::string& path, bool flip = true);
    static Ref<MeshSource> LoadMeshSource(const std::string& path);
    static Ref<Environment> LoadEnvironmentMap(const std::string& path);

   private:
    static void TraverseNodes(Ref<MeshSource>& ms, aiNode* node, const Matrix4& parentTransform = Matrix4::IDENTITY, uint32_t level = 0);
    static void TraverseNodesAnim(Ref<MeshSource>& ms, aiNode* node, Ref<Joint>& joint);
};
}  // namespace Ethereal
