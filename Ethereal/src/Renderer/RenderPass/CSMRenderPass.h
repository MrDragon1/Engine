#pragma once
#include "Renderer/RenderPass.h"

namespace Ethereal
{
    class CSMRenderPass : public RenderPass {
      public:
        CSMRenderPass() = default;
        ~CSMRenderPass() = default;

        void Init(uint32_t width, uint32_t height) override;
        void Draw() override;

        void OnResize(uint32_t width, uint32_t height) override;

      private:
        const int m_Cascaded = 4;
        const int m_ShadowMapSize = 1024;

        Ref<Shader> m_Shader;

        std::vector<Ref<Texture>> m_ShadowMaps;
        std::vector<int> m_Distance;
    };

}  // namespace Ethereal
