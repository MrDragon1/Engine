#include "EnvironmentMapRenderPass.h"

#include "Base/GlobalContext.h"

#include "Utils/StringUtils.h"
namespace Ethereal {
void EnvironmentMapRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();

    ShaderSourceString source;
    source[ShaderType::VERTEX] =
        Utils::ReadFileAndSkipBOM("assets/shaders/IBL/EquirectangularToCubeMap.vert");
    source[ShaderType::FRAGMENT] =
        Utils::ReadFileAndSkipBOM("assets/shaders/IBL/EquirectangularToCubeMap.frag");
    mEquirectangularToCubeMapPipeline.program =
        api->CreateProgram("EQUIRECTANGULARTOCUBEMAP", source);
    source[ShaderType::VERTEX] = Utils::ReadFileAndSkipBOM("assets/shaders/IBL/Prefilter.vert");
    source[ShaderType::FRAGMENT] = Utils::ReadFileAndSkipBOM("assets/shaders/IBL/Prefilter.frag");
    mPrefilterPipeline.program = api->CreateProgram("PREFILTER", source);

    source[ShaderType::VERTEX] =
        Utils::ReadFileAndSkipBOM("assets/shaders/IBL/IrradianceConvolution.vert");
    source[ShaderType::FRAGMENT] =
        Utils::ReadFileAndSkipBOM("assets/shaders/IBL/IrradianceConvolution.frag");
    mConvolutionPipeline.program = api->CreateProgram("IRRADIANCECONVOLUTION", source);

    // ShaderSource source;
    // source[ShaderType::VERTEX] = EQUIRECTANGULARTOCUBEMAP_VERT;
    // source[ShaderType::FRAGMENT] = EQUIRECTANGULARTOCUBEMAP_FRAG;
    // mEquirectangularToCubeMapPipeline.program =
    //     api->CreateProgram("EQUIRECTANGULARTOCUBEMAP", source);
    // source[ShaderType::VERTEX] = PREFILTER_VERT;
    // source[ShaderType::FRAGMENT] = PREFILTER_FRAG;
    // mPrefilterPipeline.program = api->CreateProgram("PREFILTER", source);
    // source[ShaderType::VERTEX] = IRRADIANCECONVOLUTION_VERT;
    // source[ShaderType::FRAGMENT] = IRRADIANCECONVOLUTION_FRAG;
    // mConvolutionPipeline.program = api->CreateProgram("IRRADIANCECONVOLUTION", source);

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;

    auto dummyTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                                       TextureType::TEXTURE_2D);
    mRenderTarget =
        api->CreateRenderTarget(TargetBufferFlags::COLOR0, width, height, {dummyTex}, {}, {});

    mCube = RenderResource::Cube;

    mEnvironmentCubeMap = api->CreateTexture(1, 32, 32, 6, TextureFormat::R32G32B32A32_HDR, usage,
                                             TextureType::TEXTURE_CUBEMAP);
    mReflectionCubeMap = api->CreateTexture(10, 512, 512, 6, TextureFormat::R32G32B32A32_HDR, usage,
                                            TextureType::TEXTURE_CUBEMAP);
    api->GenerateMipmaps(mReflectionCubeMap);
    mTextureCube = api->CreateTexture(1, 512, 512, 6, TextureFormat::R32G32B32A32_HDR, usage,
                                      TextureType::TEXTURE_CUBEMAP);

    mEquirectangularToCubeMapPipeline.rasterState.depthFunc = RasterState::DepthFunc::LE;

    mUniformBuffer = api->CreateBufferObject(mUniformInterfaceBuffer.GetSize(),
                                             BufferObjectBinding::UNIFORM, BufferUsage::STATIC);
    api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);

    mSamplerGroup = api->CreateSamplerGroup(1);
    mSamplerGroupDesc = SamplerGroupDescriptor(1);

    mParams.flags.clearMask = TargetBufferFlags::COLOR0;
    mParams.viewport = Viewport(0, 0, width, height);

    mEquirectangularToCubeMapPipeline.rasterState.colorTargetCount = 1;
    mPrefilterPipeline.rasterState.colorTargetCount = 1;
    mConvolutionPipeline.rasterState.colorTargetCount = 1;
}

void EnvironmentMapRenderPass::Draw() {
    Matrix4 captureProjection = Math::Perspective(Math::Radians(90.0f), 1.0f, 0.1f, 10.0f);
    Matrix4 captureViews[] = {Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(1.0f, 0.0f, 0.0f),
                                           Vector3(0.0f, -1.0f, 0.0f)),
                              Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(-1.0f, 0.0f, 0.0f),
                                           Vector3(0.0f, -1.0f, 0.0f)),
                              Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f),
                                           Vector3(0.0f, 0.0f, 1.0f)),
                              Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f),
                                           Vector3(0.0f, 0.0f, -1.0f)),
                              Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f),
                                           Vector3(0.0f, -1.0f, 0.0f)),
                              Math::LookAt(Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f),
                                           Vector3(0.0f, -1.0f, 0.0f))};
    auto api = GlobalContext::GetDriverApi();
    auto uniformManager = GlobalContext::GetUniformManager();

    auto& s = mUniformInterfaceBuffer.Edit();
    // Generate Environment CubeMap
    s.ProjectionMatrix = captureProjection;
    for (uint16_t i = 0; i < 6; i++) {
        mSamplerGroupDesc[0] = SamplerDescriptor{
            .texture = mInputTexture,
            .params = SamplerParams::Default(),
            .binding = 0,
        };
        s.ViewMatrix = captureViews[i];

        api->SetRenderTargetAttachment(mRenderTarget,
                                       {.handle = mTextureCube, .level = 0, .layer = i},
                                       TargetBufferFlags::COLOR0);

        api->BeginRenderPass(mRenderTarget, mParams);
        api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);
        api->BindUniformBuffer(0, mUniformBuffer);

        api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
        api->BindSamplerGroup(0, mSamplerGroup);

        api->Draw(mCube->GetMeshSource()->GetRenderPrimitive(), mEquirectangularToCubeMapPipeline);

        api->EndRenderPass();
    }

    // Convolution
    for (uint16_t i = 0; i < 6; i++) {
        mSamplerGroupDesc[0] = SamplerDescriptor{
            .texture = mTextureCube,
            .params = SamplerParams::Default(),
            .binding = 0,
        };
        s.ViewMatrix = captureViews[i];

        api->SetRenderTargetAttachment(mRenderTarget,
                                       {.handle = mEnvironmentCubeMap, .level = 0, .layer = i},
                                       TargetBufferFlags::COLOR0);
        api->BeginRenderPass(mRenderTarget, mParams);
        api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(), 0);
        api->BindUniformBuffer(0, mUniformBuffer);

        api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
        api->BindSamplerGroup(0, mSamplerGroup);
        api->Draw(mCube->GetMeshSource()->GetRenderPrimitive(), mConvolutionPipeline);

        api->EndRenderPass();
    }

    // Generate Reflection CubeMap
    // here, we reuse the mTextureCube

    // Generate Pre-filter CubeMap
    unsigned int maxMipLevels = 10;
    for (uint32_t mip = 0; mip < maxMipLevels; ++mip) {
        // resize frame buffer according to mip-level size.
        unsigned int mipWidth = static_cast<unsigned int>(512 * std::pow(0.5, mip));
        unsigned int mipHeight = static_cast<unsigned int>(512 * std::pow(0.5, mip));
        float roughness = (float)mip / (float)(maxMipLevels - 1);

        s.Roughness = roughness;

        for (uint32_t i = 0; i < 6; i++) {
            mSamplerGroupDesc[0] = SamplerDescriptor{
                .texture = mTextureCube,
                .params = SamplerParams::Default(),
                .binding = 0,
            };
            s.ViewMatrix = captureViews[i];

            api->SetRenderTargetAttachment(mRenderTarget,
                                           {.handle = mReflectionCubeMap, .level = mip, .layer = i},
                                           TargetBufferFlags::COLOR0);
            api->BeginRenderPass(mRenderTarget, mParams);
            api->UpdateBufferObject(mUniformBuffer, mUniformInterfaceBuffer.ToBufferDescriptor(),
                                    0);
            api->BindUniformBuffer(0, mUniformBuffer);

            api->UpdateSamplerGroup(mSamplerGroup, mSamplerGroupDesc);
            api->BindSamplerGroup(0, mSamplerGroup);
            api->Draw(mCube->GetMeshSource()->GetRenderPrimitive(), mPrefilterPipeline);
            api->EndRenderPass();
        }
    }
}

void EnvironmentMapRenderPass::OnResize(uint32_t width, uint32_t height) {
    ET_CORE_INFO("EnvironmentMapRenderPass::OnResize");
    mParams.viewport = Viewport(0, 0, width, height);
}
}  // namespace Ethereal