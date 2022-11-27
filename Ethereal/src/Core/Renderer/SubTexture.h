#pragma once
#include "Core/Renderer/Texture.h"
#include "Base/Math/Vector.h"
namespace Ethereal
{
    class SubTexture2D : public RefCounted {
      public:
        SubTexture2D(const Ref<Texture2D>& texture, const Vector2& min, const Vector2& max);

        const Ref<Texture2D> GetTexture() const { return m_Texture; }
        const Vector2* GetTexCoords() const { return m_Coords; }

        static Ref<SubTexture2D> CreateFromCoords(const Ref<Texture2D>& texture, const Vector2& coords, const Vector2& cellSize,
                                                  const Vector2& spriteSize = {1, 1});

      private:
        Ref<Texture2D> m_Texture;
        Vector2 m_Coords[4];
    };
}  // namespace Ethereal