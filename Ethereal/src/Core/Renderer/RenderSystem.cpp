#include "RenderSystem.h"

#include "Core/Asset/AssetManager.h"
#include "Core/Renderer/RenderResource.h"
#include "Ethereal/src/Base/GlobalContext.h"
#include "Ethereal/src/Core/Scene/Components.h"
#include "Ethereal/src/Core/Scene/Entity.h"
#include "Ethereal/src/Core/Scene/Scene.h"
#include "Ethereal/src/Core/Scene/ScriptableEntity.h"
// Temporary
#include "Resource/ResourceLoader.h"
#include "Utils/AssetLoader.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_world.h"

// Test Backend
#include "Shader.h"
#include "Utils/AssetLoader.h"

namespace Ethereal {
static uint32_t toUintColor(Vector4 color) {
    color.x = Math::Clamp(color.x, 0, 1);
    color.y = Math::Clamp(color.y, 0, 1);
    color.z = Math::Clamp(color.z, 0, 1);
    color.w = Math::Clamp(color.w, 0, 1);
    uint32_t r = color.x * 255.0f;
    uint32_t g = color.y * 255.0f;
    uint32_t b = color.z * 255.0f;
    uint32_t a = color.w * 255.0f;
    return (r << 0) | (g << 8) | (b << 16) | (a << 24);
}

void createBitmap(Ref<Backend::DriverApi> dapi, Backend::TextureHandle texture, int baseWidth, int baseHeight, int level, int layer, Vector3 color,
                  bool flipY) {
    using namespace Backend;
    auto cb = [](void* buffer, size_t size, void* user) { free(buffer); };
    const int width = baseWidth >> level;
    const int height = baseHeight >> level;
    const size_t size0 = height * width * 4;
    uint8_t* buffer0 = (uint8_t*)calloc(size0, 1);
    PixelBufferDescriptor pb(buffer0, size0, PixelDataFormat::RGBA, PixelDataType::UBYTE);

    const Vector3 foreground = color;
    const Vector3 background = Vector3(1, 1, 0);

    uint32_t* texels = (uint32_t*)buffer0;
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < width; col++) {
            Vector2 uv = {(col - width / 2.0f) / width, (row - height / 2.0f) / height};
            const float d = abs(uv.x);
            const float triangleWidth = uv.y >= -.3 && uv.y <= .3 ? (.4f / .6f * uv.y + .2f) : 0;
            const float t = d < triangleWidth ? 1.0 : 0.0;
            const Vector3 color = foreground * t + background * (1 - t);
            int rowFlipped = flipY ? (height - 1) - row : row;
            texels[rowFlipped * width + col] = toUintColor(Vector4(color, 1.0f));
        }
    }

    // Upload to the GPU.
    dapi->SetTextureData(texture, level, 0, 0, layer, width, height, 1, std::move(pb));
}

Ref<Backend::RenderPrimitive> GetTrianglePrimitive(Ref<Backend::DriverApi> api) {
    using namespace Backend;
    Vector2 gVertices[4] = {{0.5, 0.5}, {0.5, -0.5}, {-0.5, -0.5}, {-0.5, 0.5}};
    Vector2 gUV[4] = {{1, 1}, {1, 0}, {0, 0}, {0, 1}};
    uint32_t gIndices[6] = {0, 1, 3, 1, 2, 3};

    uint8_t mVertexCount = 4;
    AttributeArray attributes = {Backend::Attribute{
                                     .offset = 0,
                                     .stride = sizeof(Vector2),
                                     .buffer = 0,
                                     .type = ElementType::FLOAT2,
                                     .flags = 0,
                                 },
                                 Backend::Attribute{
                                     .offset = 0,
                                     .stride = sizeof(Vector2),
                                     .buffer = 1,
                                     .type = ElementType::FLOAT2,
                                     .flags = 0,
                                 }};

    const size_t size = sizeof(Vector2) * 4;
    BufferObjectHandle bufferObject = api->CreateBufferObject(size, BufferObjectBinding::VERTEX, BufferUsage::STATIC);
    BufferDescriptor vertexBufferDesc(gVertices, size);
    api->UpdateBufferObject(bufferObject, std::move(vertexBufferDesc), 0);

    BufferObjectHandle bufferObject2 = api->CreateBufferObject(size, BufferObjectBinding::VERTEX, BufferUsage::STATIC);
    BufferDescriptor vertexBufferDesc2(gUV, size);
    api->UpdateBufferObject(bufferObject2, std::move(vertexBufferDesc2), 0);

    VertexBufferHandle vertexBuffer = api->CreateVertexBuffer(attributes, mVertexCount, 2, 2);
    api->SetVertexBufferObject(vertexBuffer, 0, bufferObject);
    api->SetVertexBufferObject(vertexBuffer, 1, bufferObject2);

    uint8_t mIndexCount = 6;
    ElementType elementType = ElementType::UINT;
    static_assert(sizeof(uint32_t) == 4);
    IndexBufferHandle indexBuffer = api->CreateIndexBuffer(elementType, mIndexCount, BufferUsage::STATIC);
    BufferDescriptor indexBufferDesc(gIndices, sizeof(uint32_t) * mIndexCount);
    api->UpdateIndexBuffer(indexBuffer, std::move(indexBufferDesc), 0);

    return api->CreateRenderPrimitive(vertexBuffer, indexBuffer, PrimitiveType::TRIANGLES, 0, 0, mIndexCount - 1, mIndexCount);
}

RenderSystem::RenderSystem() {}

void RenderSystem::Init() {
    m_Width = 1280;
    m_Height = 720;

    m_DrawLists = &RenderPass::m_DrawLists;

    m_BuildinData = new BuildinData();

    m_EnvironmentMapRenderPass = Ref<EnvironmentMapRenderPass>::Create();
    m_EnvironmentMapRenderPass->Init(m_Width, m_Height);
    m_MainCameraRenderPass = Ref<MainCameraRenderPass>::Create();
    m_MainCameraRenderPass->Init(m_Width, m_Height);
    m_ShadowMapRenderPass = Ref<ShadowMapRenderPass>::Create();
    m_ShadowMapRenderPass->Init(m_Width, m_Height);
    m_SkyboxRenderPass = Ref<SkyboxRenderPass>::Create();
    m_SkyboxRenderPass->Init(m_Width, m_Height);
    m_BloomRenderPass = Ref<BloomRenderPass>::Create();
    m_BloomRenderPass->Init(m_Width, m_Height);
    m_CSMRenderPass = Ref<CSMRenderPass>::Create();
    m_CSMRenderPass->Init(m_Width, m_Height);

    // Must after m_EnvironmentMapRenderPass Init
    m_BuildinData->Environment = AssetManager::GetAsset<Environment>("skyboxs/Newport_Loft_Ref.hdr");

    m_MainImage = m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0);

    m_UniformBufferSet = UniformBufferSet::Create(1);
    m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(CameraData), 0), 0);
    m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(ShadowData), 1), 0);
    m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(SceneData), 2), 0);
    m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(RendererData), 3), 0);
    m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(MaterialData), 4), 0);
    m_UniformBufferSet->Set(UniformBuffer::Create(sizeof(FogData), 5), 0);

    mDriver = new Backend::Driver(Backend::BackendType::OPENGL);
    mTrianglePrimitive = GetTrianglePrimitive(mDriver->GetApi());

    using namespace Backend;
    Ref<TextureData> data = Ref<TextureData>::Create();
    TextureLoader::LoadPath("assets/textures/awesomeface.png", data);
    TextureHandle texture = mDriver->GetApi()->CreateTexture(1, data->m_width, data->m_height, data->m_depth, TextureFormat::R8G8B8A8_UNORM,
                                                             TextureUsage::DEFAULT, TextureType::TEXTURE_2D);
    size_t bufferSize = data->m_width * data->m_height * 4;
    PixelBufferDescriptor pixelDesc(data->m_pixels, bufferSize, PixelDataFormat::RGBA, PixelDataType::UBYTE);  // The image loaded is 4 channel
    mDriver->GetApi()->SetTextureData(texture, 0, 0, 0, 0, data->m_width, data->m_height, data->m_depth, pixelDesc);

    mSamplerGroup = mDriver->GetApi()->CreateSamplerGroup(1);
    Backend::SamplerGroupDescriptor desc(1);
    desc[0].texture = texture;
    desc[0].binding = 0;
    desc[0].params = SamplerParams{
        .filterMag = SamplerMagFilter::LINEAR,
        .filterMin = SamplerMinFilter::LINEAR,
        .wrapS = SamplerWrapMode::REPEAT,
        .wrapT = SamplerWrapMode::REPEAT,
        .wrapR = SamplerWrapMode::REPEAT,
        .compareMode = SamplerCompareMode::NONE,
    };

    mDriver->GetApi()->UpdateSamplerGroup(mSamplerGroup, desc);

    Backend::ShaderSource source;
    source[Backend::ShaderType::VERTEX] = TEST_VERT;
    source[Backend::ShaderType::FRAGMENT] = TEST_FRAG;
    mPipelineState.program = mDriver->GetApi()->CreateProgram("Test", source);

    // Create two Textures.
    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    mTextureA = mDriver->GetApi()->CreateTexture(1, 512, 512, 1, TextureFormat::R8G8B8A8_UNORM, usage, TextureType::TEXTURE_2D);
    mTextureB = mDriver->GetApi()->CreateTexture(1, 512, 512, 1, TextureFormat::R8G8B8A8_UNORM, usage, TextureType::TEXTURE_2D);
    Backend::MRT mrt;
    mrt[0] = {mTextureA};
    mrt[1] = {mTextureB};

    // Create a RenderTarget with two attachments.
    mRenderTarget = mDriver->GetApi()->CreateRenderTarget(TargetBufferFlags::COLOR0 | TargetBufferFlags::COLOR1,
                                                          // The width and height must match the width and height of the respective
                                                          // mip level (at least for OpenGL).
                                                          512,  // width
                                                          512,  // height
                                                          mrt,  // color
                                                          {},   // depth
                                                          {});  // stencil
}

void RenderSystem::Draw(TimeStamp ts) {
    LoadProjectSettings();

    // m_MainCameraRenderPass->m_Framebuffer->Bind();
    // RenderCommand::SetClearColor({0.1f, 0.1f, 0.1f, 1});
    // RenderCommand::Clear();

    Backend::RenderPassParams params = {};
    params.clearColor = {0.3, 0.3, 0.3, 1.0};

    mDriver->GetApi()->BindSamplerGroup(0, mSamplerGroup);

    mDriver->GetApi()->BeginRenderPass(mRenderTarget, params);

    mDriver->GetApi()->Draw(mTrianglePrimitive, mPipelineState);
    mDriver->GetApi()->EndRenderPass();

    // m_MainCameraRenderPass->m_Framebuffer->Unbind();
    // m_MainImage = m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0);

    //// m_ShadowMapRenderPass->SetLightPosition();
    // m_CSMRenderPass->Draw();

    //// m_ShadowMapRenderPass->Draw();
    //// m_ShadowMapRenderPass->m_Framebuffer->GetDepthAttachment()->Bind(5);

    // m_MainCameraRenderPass->SetCSMData(m_CSMRenderPass->GetData());
    //// TODO: Radiance and Irradiance may used as reversed wrongly in shader!!!
    // m_Environment->RadianceMap->Bind(17);
    // m_Environment->IrradianceMap->Bind(16);
    // RenderResource::BRDFLutTexture->Bind(15);
    // m_MainCameraRenderPass->Draw();

    //// TODO : make skybox render pass a subpass of main camera render pass
    // m_MainCameraRenderPass->m_Framebuffer->Bind();
    // m_Environment->IrradianceMap->Bind(0);
    // m_SkyboxRenderPass->Draw();
    // m_MainCameraRenderPass->m_Framebuffer->Unbind();

    //// For postprocessing
    // auto mainImg = m_MainCameraRenderPass->m_Framebuffer->GetColorAttachment(0);
    // m_MainImage = mainImg;
    // if (m_BloomRenderPass->GetEnabled()) {
    //     m_BloomRenderPass->SetMainImage(mainImg);
    //     m_BloomRenderPass->Draw();

    //    m_MainImage = m_BloomRenderPass->GetBloomImage();
    //}

    m_DrawLists->MeshTransformMap.clear();
    m_DrawLists->StaticMeshDrawList.clear();
}

void RenderSystem::OnResize() {
    m_Height = GlobalContext::GetViewportSize().y;
    m_Width = GlobalContext::GetViewportSize().x;
    m_ShadowMapRenderPass->OnResize(m_Width, m_Height);
    m_MainCameraRenderPass->OnResize(m_Width, m_Height);
}

uint64_t RenderSystem::GetMainImage() { return m_MainImage->GetRendererID(); }

int RenderSystem::GetMousePicking(int x, int y) { return m_MainCameraRenderPass->GetMousePicking(x, y); }

void RenderSystem::SubmitStaticMesh(Ref<StaticMesh> staticMesh, Ref<MaterialTable> materialTable, uint32_t EntityID, const Matrix4& transform) {
    Ref<MeshSource> meshSource = staticMesh->GetMeshSource();
    const auto& submeshData = meshSource->GetSubmeshes();
    for (uint32_t submeshIndex : staticMesh->GetSubmeshes()) {
        Matrix4 submeshTransform = transform * submeshData[submeshIndex].Transform;

        const auto& submeshes = staticMesh->GetMeshSource()->GetSubmeshes();
        uint32_t materialIndex = submeshes[submeshIndex].MaterialIndex;
        Ref<MaterialAsset> material = materialTable->GetMaterial(materialIndex);
        AssetHandle materialHandle = material->Handle;

        MeshKey meshKey = {staticMesh->Handle, materialHandle, submeshIndex, false, EntityID};
        auto& transformStorage = m_DrawLists->MeshTransformMap[meshKey].Transforms.emplace_back();
        transformStorage.Transform = submeshTransform;
        // Main geo
        {
            bool isTransparent = material->IsTransparent();
            if (isTransparent) ET_CORE_WARN("Only support untransparent material");
            auto& destDrawList = m_DrawLists->StaticMeshDrawList;
            auto& dc = destDrawList[meshKey];
            dc.StaticMesh = staticMesh;
            dc.SubmeshIndex = submeshIndex;
            dc.MaterialTable = materialTable;
            dc.InstanceCount++;
        }
    }
}

void RenderSystem::SubmitMesh(Ref<Mesh> mesh, Ref<MaterialTable> materialTable, uint32_t EntityID, const Matrix4& transform) {
    Ref<MeshSource> meshSource = mesh->GetMeshSource();
    const auto& submeshData = meshSource->GetSubmeshes();
    for (uint32_t submeshIndex : mesh->GetSubmeshes()) {
        Matrix4 submeshTransform = transform * submeshData[submeshIndex].Transform;

        const auto& submeshes = mesh->GetMeshSource()->GetSubmeshes();
        uint32_t materialIndex = submeshes[submeshIndex].MaterialIndex;
        Ref<MaterialAsset> material = materialTable->GetMaterial(materialIndex);
        AssetHandle materialHandle = material->Handle;

        MeshKey meshKey = {mesh->Handle, materialHandle, submeshIndex, false, EntityID};
        auto& transformStorage = m_DrawLists->MeshTransformMap[meshKey].Transforms.emplace_back();
        transformStorage.Transform = submeshTransform;
        // Main geo
        {
            bool isTransparent = material->IsTransparent();
            if (isTransparent) ET_CORE_WARN("Only support untransparent material");
            auto& destDrawList = m_DrawLists->MeshDrawList;
            auto& dc = destDrawList[meshKey];
            dc.Mesh = mesh;
            dc.SubmeshIndex = submeshIndex;
            dc.MaterialTable = materialTable;
            dc.InstanceCount++;
        }
    }
}

void RenderSystem::SubmitRenderSceneData() {
    m_CSMRenderPass->SetNearFarPlane(m_ShaderCommonData.CameraData.NearPlane, m_ShaderCommonData.CameraData.FarPlane);
    m_CSMRenderPass->SetFOV(m_ShaderCommonData.RenderSceneData.FOV);
    m_CSMRenderPass->SetAspectRatio(m_ShaderCommonData.RenderSceneData.AspectRatio);
    m_CSMRenderPass->SetLightDir(m_ShaderCommonData.SceneData.DirectionalLight.Direction);

    m_Environment = m_ShaderCommonData.RenderSceneData.Environment;

    UpdateUniformData();
}

std::pair<Ref<TextureCube>, Ref<TextureCube>> RenderSystem::CreateEnvironmentMap(const std::string& path) {
    m_EnvironmentMapRenderPass->m_Path = path;
    m_EnvironmentMapRenderPass->Draw();
    // TODO: Copy the Ref<TextureCube> otherwise every Environment will point to the same Ref
    return {m_EnvironmentMapRenderPass->m_EnvironmentCubeMap, m_EnvironmentMapRenderPass->m_ReflectionCubeMap};
}

void RenderSystem::LoadProjectSettings() {
    // Bloom Settings
    m_BloomRenderPass->GetEnabled() = Project().GetSettings().bloomSetting.enabled;
    m_BloomRenderPass->GetIntensity() = Project().GetSettings().bloomSetting.intensity;
    m_BloomRenderPass->GetThreshold() = Project().GetSettings().bloomSetting.threshold;
    m_BloomRenderPass->GetKnee() = Project().GetSettings().bloomSetting.knee;
}

void RenderSystem::UpdateUniformData() {
    // 从editor传过来的数据在此写入UBO
    m_UniformBufferSet->Get(0, 0)->SetData(&m_ShaderCommonData.CameraData, sizeof(CameraData));
    m_UniformBufferSet->Get(0, 1)->SetData(&m_ShaderCommonData.ShadowData, sizeof(ShadowData));
    m_UniformBufferSet->Get(0, 2)->SetData(&m_ShaderCommonData.SceneData, sizeof(SceneData));
    m_UniformBufferSet->Get(0, 3)->SetData(&m_ShaderCommonData.RendererData, sizeof(RendererData));
    m_UniformBufferSet->Get(0, 4)->SetData(&m_ShaderCommonData.MaterialData, sizeof(MaterialData));
    m_UniformBufferSet->Get(0, 5)->SetData(&m_ShaderCommonData.FogData, sizeof(FogData));
}

}  // namespace Ethereal