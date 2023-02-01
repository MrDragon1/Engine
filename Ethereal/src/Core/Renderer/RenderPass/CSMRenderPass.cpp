#include "CSMRenderPass.h"
#include <Base/GlobalContext.h>

namespace Ethereal
{
    void CSMRenderPass::Init(uint32_t width, uint32_t height) {
        // Generate ShadowMaps
        TextureSpecification spec;
        spec.Depth = GlobalContext::GetRenderSystem().GetShaderCommonData().ShadowData.CascadeCount + 1;
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

        // Draw Shadow Map
        const auto& staticMeshDrawList = m_DrawLists.StaticMeshDrawList;
        const auto& meshTransformMap = m_DrawLists.MeshTransformMap;


        auto count = GlobalContext::GetRenderSystem().GetShaderCommonData().ShadowData.CascadeCount;
        auto& splits = GlobalContext::GetRenderSystem().GetShaderCommonData().ShadowData.CascadeSplits;
        for(int i = 0; i < count; i++) {
            splits[i].x = m_Distance[i]; // Refer to the comment in RenderSystem.h of ShadowData
        }
        CalculateLightSpaceMatrices();

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

        m_Framebuffer->Unbind();
    }

    void CSMRenderPass::OnResize(uint32_t width, uint32_t height) {}

    std::vector<Vector4> CSMRenderPass::GetFrustumCornersWorldSpace(const Matrix4& projview) {
        const auto inv = Math::Inverse(projview);

        std::vector<Vector4> frustumCorners;
        for (unsigned int x = 0; x < 2; ++x) {
            for (unsigned int y = 0; y < 2; ++y) {
                for (unsigned int z = 0; z < 2; ++z) {
                    const Vector4 pt = inv * Vector4(2.0f * x - 1.0f, 2.0f * y - 1.0f, 2.0f * z - 1.0f, 1.0f);
                    frustumCorners.push_back(pt / pt.w);
                }
            }
        }

        return frustumCorners;
    }

    std::vector<Vector4> CSMRenderPass::GetFrustumCornersWorldSpace(const Matrix4& proj, const Matrix4& view) {
        return GetFrustumCornersWorldSpace(proj * view);
    }

    Matrix4 CSMRenderPass::GetLightSpaceMatrix(float nearPlane, float farPlane) {
        const auto proj = Math::Perspective(Math::Radians(m_Data.FOV), m_Data.AspectRatio, nearPlane, farPlane);
        const auto corners = GetFrustumCornersWorldSpace(proj, GlobalContext::GetRenderSystem().GetShaderCommonData().CameraData.ViewMatrix);

        Vector3 center = Vector3(0, 0, 0);
        for (const auto& v : corners) {
            center += Vector3(v.x,v.y,v.z);
        }
        center /= corners.size();

        const auto lightView = Math::LookAt(center + m_Data.LightDir, center, Vector3(0.0f, 1.0f, 0.0f));

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

        const Matrix4 lightProjection = Math::Ortho(minX, maxX, minY, maxY, minZ, maxZ);

        return lightProjection * lightView;
    }

    void CSMRenderPass::CalculateLightSpaceMatrices() {
        std::vector<Matrix4> ret;
        auto count = GlobalContext::GetRenderSystem().GetShaderCommonData().ShadowData.CascadeCount;
        auto& matrices = GlobalContext::GetRenderSystem().GetShaderCommonData().ShadowData.DirLightMatrices;

        for (size_t i = 0; i < count + 1; ++i) {
            if (i == 0) {
                matrices[i] = GetLightSpaceMatrix(GlobalContext::GetRenderSystem().GetShaderCommonData().CameraData.NearPlane, m_Distance[i]);
            } else if (i < m_Distance.size()) {
                matrices[i] = GetLightSpaceMatrix(m_Distance[i - 1], m_Distance[i]);
            } else {
                matrices[i] = GetLightSpaceMatrix(m_Distance[i - 1], GlobalContext::GetRenderSystem().GetShaderCommonData().CameraData.FarPlane);
            }
        }
    }

    void CSMRenderPass::SetNearFarPlane(float nearPlane, float farPlane)  {
        GlobalContext::GetRenderSystem().GetShaderCommonData().CameraData.NearPlane = nearPlane;
        GlobalContext::GetRenderSystem().GetShaderCommonData().CameraData.FarPlane = farPlane;
        // TODO: automatically generate distance according to m_Cascaded
        m_Distance.clear();
        m_Distance.push_back(farPlane / 50.0f);
        m_Distance.push_back(farPlane / 25.0f);
        m_Distance.push_back(farPlane / 10.0f);
        m_Distance.push_back(farPlane / 2.0f);
    }
}  // namespace Ethereal