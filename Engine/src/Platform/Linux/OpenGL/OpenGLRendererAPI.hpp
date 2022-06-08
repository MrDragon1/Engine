#pragma once

#include "Core/Renderer/RendererAPI.hpp"

namespace Engine
{
    class OpenGLRendererAPI : public RendererAPI
    {
        virtual void SetClearColor(const glm::vec4& color) override;
        virtual void Clear() override;

        virtual void DrawIndexed(const Ref<VertexArray>& vertexArray) override;
    };
}