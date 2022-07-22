#pragma once

#include "Ethereal/Renderer/Texture.hpp"
#include "Ethereal/Renderer/Common.hpp"
#include "Ethereal/Core/Base/Singleton.hpp"
namespace Ethereal
{
    class TextureManager : public Singleton<TextureManager>
    {
    public:
        static Ref<Texture> GetTexture(const std::string& path);
        static Ref<Texture> AddTexture(const std::string& path);
    private:
        static std::unordered_map<std::string, Ref<Texture>> m_TextureMap;
    };
}  // namespace Ethereal