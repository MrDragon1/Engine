#include "pch.h"
#include "SubTexture.h"

namespace Ethereal {
    SubTexture2D::SubTexture2D(const Ref<Texture2D>& texture, const glm::vec2& min, const glm::vec2& max) : m_Texture(texture) {
        m_Coords[0] = {min.x,min.y};
        m_Coords[1] = {max.x,min.y};
        m_Coords[2] = {max.x,max.y};
        m_Coords[3] = {min.x,max.y};
    }

    Ref<SubTexture2D> SubTexture2D::CreateFromCoords(const Ref<Texture2D>& texture, const glm::vec2& coords, const glm::vec2& cellSize, const glm::vec2& spriteSize)
    {
        glm::vec2 min = { (coords.x * cellSize.x) / texture->GetWidth(), (coords.y * cellSize.y) / texture->GetHeight() };
        glm::vec2 max = { ((coords.x + spriteSize.x) * cellSize.x) / texture->GetWidth(), ((coords.y + spriteSize.y) * cellSize.y) / texture->GetHeight() };
        return Ref<SubTexture2D>::Create(texture, min, max);
    }
}  // namespace Ethereal
