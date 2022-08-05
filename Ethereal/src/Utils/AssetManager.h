#pragma once
#include "Renderer/Texture.h"
#include "Core/Base/Singleton.h"
namespace Ethereal
{
    class TextureManager : public Singleton<TextureManager> {
      public:
        static Ref<Texture> GetTexture(const std::string& path);
        static Ref<Texture> AddTexture(const std::string& path);

      private:
        static std::unordered_map<std::string, Ref<Texture>> m_TextureMap;
    };

    class MeshManager : public Singleton<MeshManager> {
      public:
    };
}  // namespace Ethereal