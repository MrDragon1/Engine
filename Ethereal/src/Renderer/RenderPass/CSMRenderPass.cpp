#include "CSMRenderPass.h"

namespace Ethereal
{
    void CSMRenderPass::Init(uint32_t width, uint32_t height) {
        // Generate ShadowMaps
        for (int i = 0; i < m_Cascaded; i++) {
            Ref<TextureData> data = Ref<TextureData>::Create();
            data->m_height = m_ShadowMapSize;
            data->m_width = m_ShadowMapSize;
            data->m_depth = 1;
            data->m_type = ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D;
            data->m_format = ETHEREAL_PIXEL_FORMAT::DEPTH;
            auto map = Texture2D::Create(data);
            m_ShadowMaps.emplace_back(map);
        }
    }

    void CSMRenderPass::Draw() {}

    void CSMRenderPass::OnResize(uint32_t width, uint32_t height) {}

}  // namespace Ethereal