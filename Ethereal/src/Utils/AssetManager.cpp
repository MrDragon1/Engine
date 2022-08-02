#include "AssetManager.h"
#include "AssetLoader.h"
namespace Ethereal
{
    std::unordered_map<std::string, Ref<Texture>> TextureManager::m_TextureMap;

    Ref<Texture> TextureManager::GetTexture(const std::string& path) {
        auto it = m_TextureMap.find(path);
        if (it != m_TextureMap.end()) {
            return it->second;
        } else {
            ET_CORE_WARN("Texture not found: {0}", path);
            return nullptr;
        }
    }

    Ref<Texture> TextureManager::AddTexture(const std::string& path) {
        auto it = m_TextureMap.find(path);
        if (it == m_TextureMap.end()) {
            Ref<Texture> texture = Texture2D::Create(path);
            if (texture->IsLoaded()) {
                m_TextureMap[path] = texture;
                return texture;
            } else
                return nullptr;
        } else {
            // ET_CORE_WARN("Texture already exists: {0}.", path);
            return it->second;
        }
    }

}  // namespace Ethereal