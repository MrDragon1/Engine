#include "CSMRenderPass.h"

namespace Ethereal
{
    void CSMRenderPass::Init(uint32_t width, uint32_t height) {
        // Generate ShadowMaps
        TextureSpecification spec;
        spec.Depth = m_Data.Cascaded + 1;
        spec.Type = ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_2D_ARRAY;
        spec.Format = ETHEREAL_PIXEL_FORMAT::DEPTH;

        FramebufferSpecification fbSpec;
        fbSpec.DepthAttachment.SetAttachmentSpec(spec);
        fbSpec.Width = m_Data.ShadowMapSize;
        fbSpec.Height = m_Data.ShadowMapSize;

        m_Framebuffer = Framebuffer::Create(fbSpec);
        m_Data.ShadowMap = m_Framebuffer->GetDepthAttachment();

        m_Shader = Shader::Create(m_ShaderPath);
    }

    void CSMRenderPass::Draw() {
        m_Shader->Bind();
        m_Framebuffer->Bind();
        RenderCommand::SetClearColor({0, 0, 0, 0});
        RenderCommand::Clear();
        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::FRONT);
        // Draw Shadow Map
        const auto& staticMeshDrawList = m_DrawLists.StaticMeshDrawList;
        const auto& meshTransformMap = m_DrawLists.MeshTransformMap;

        m_Data.Distance = m_Distance;
        m_Data.LightMatrices = GetLightSpaceMatrices();
        for (int i = 0; i < m_Data.LightMatrices.size(); i++) {
            m_Shader->SetMat4("u_LightSpaceMatrices[" + std::to_string(i) + "]", m_Data.LightMatrices[i]);
        }

        // Draw Static Mesh
        if (!staticMeshDrawList.empty()) {
            for (auto& [mk, dc] : staticMeshDrawList) {
                Ref<MeshSource> ms = dc.StaticMesh->GetMeshSource();
                Ref<MaterialTable> mt = dc.MaterialTable;
                Submesh& submesh = ms->GetSubmeshes()[dc.SubmeshIndex];

                ms->GetVertexArray()->Bind();
                m_Shader->SetMat4("u_Model", meshTransformMap.at(mk).Transforms[0].Transform);

                RenderCommand::DrawIndexed(ms->GetVertexArray(), submesh.IndexCount, reinterpret_cast<void*>(submesh.BaseIndex * sizeof(uint32_t)),
                                           submesh.BaseVertex);
            }
        }

        RenderCommand::SetCullFace(ETHEREAL_CULLFACE_TYPE::BACK);
        m_Framebuffer->Unbind();
    }

    void CSMRenderPass::OnResize(uint32_t width, uint32_t height) {}

    std::vector<glm::vec4> CSMRenderPass::GetFrustumCornersWorldSpace(const glm::mat4& projview) {
        const auto inv = glm::inverse(projview);

        std::vector<glm::vec4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const glm::vec4 pt = inv * glm::vec4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    std::vector<glm::vec4> CSMRenderPass::GetFrustumCornersWorldSpace(const glm::mat4& proj, const glm::mat4& view) {
        return GetFrustumCornersWorldSpace(proj * view);
    }

    glm::mat4 CSMRenderPass::GetLightSpaceMatrix(float nearPlane, float farPlane) {
        const auto proj = glm::perspective(glm::radians(m_Data.FOV), m_Data.AspectRatio, nearPlane, farPlane);
        const auto corners = GetFrustumCornersWorldSpace(proj, m_Data.View);

        glm::vec3 center = glm::vec3(0, 0, 0);
        for (const auto& v : corners) {
            center += glm::vec3(v);
        }
        center /= corners.size();

        const auto lightView = glm::lookAt(center + m_Data.LightDir, center, glm::vec3(0.0f, 1.0f, 0.0f));

        float minX = std::numeric_limits<float>::max();
        float maxX = std::numeric_limits<float>::min();
        float minY = std::numeric_limits<float>::max();
        float maxY = std::numeric_limits<float>::min();
        float minZ = std::numeric_limits<float>::max();
        float maxZ = std::numeric_limits<float>::min();
        for (const auto& v : corners) {
            const auto trf = lightView * v;
            minX = std::min(minX, trf.x);
            maxX = std::max(maxX, trf.x);
            minY = std::min(minY, trf.y);
            maxY = std::max(maxY, trf.y);
            minZ = std::min(minZ, trf.z);
            maxZ = std::max(maxZ, trf.z);
        }

        // Tune this parameter according to the scene
        constexpr float zMult = 10.0f;
        if (minZ < 0) {
            minZ *= zMult;
        } else {
            minZ /= zMult;
        }
        if (maxZ < 0) {
            maxZ /= zMult;
        } else {
            maxZ *= zMult;
        }

        const glm::mat4 lightProjection = glm::ortho(minX, maxX, minY, maxY, minZ, maxZ);

        return lightProjection * lightView;
    }

    std::vector<glm::mat4> CSMRenderPass::GetLightSpaceMatrices() {
        std::vector<glm::mat4> ret;
        for (size_t i = 0; i < m_Distance.size() + 1; ++i) {
            if (i == 0) {
                ret.push_back(GetLightSpaceMatrix(m_Data.NearPlane, m_Distance[i]));
            } else if (i < m_Distance.size()) {
                ret.push_back(GetLightSpaceMatrix(m_Distance[i - 1], m_Distance[i]));
            } else {
                ret.push_back(GetLightSpaceMatrix(m_Distance[i - 1], m_Data.FarPlane));
            }
        }
        return ret;
    }
}  // namespace Ethereal