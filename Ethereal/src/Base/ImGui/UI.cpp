#include "UI.h"
#include "Core/Renderer/RendererAPI.h"
#include "Platform/Windows/OpenGL/OpenGLTexture.h"

namespace Ethereal{
    namespace UI{
        void Image(const Ref<Texture2D>& texture, const ImVec2& size, const ImVec2& uv0, const ImVec2& uv1, const ImVec4& tint_col, const ImVec4& border_col)
        {
            if (RendererAPI::GetAPI() == RendererAPI::API::OpenGL)
            {
                Ref<OpenGLTexture2D> openGLTexture = texture.As<OpenGLTexture2D>();

                const ImTextureID textureID = reinterpret_cast<void*>(openGLTexture->GetRendererID());
                ImGui::Image(textureID, size, uv0, uv1, tint_col, border_col);
            }
        }

    }
}