#include "TestRenderPass.h"
#include <Base/GlobalContext.h>
#include "Utils/StringUtils.h"

namespace Ethereal {

void TestRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    ShaderSourceString source;
    source[ShaderType::VERTEX] = Utils::ReadFileAndSkipBOM("assets/shaders/Test.vert");
    source[ShaderType::FRAGMENT] = Utils::ReadFileAndSkipBOM("assets/shaders/Test.frag");
    mStaticMeshPipeline.program = api->CreateProgram("TEST", source);

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R8G8B8A8_UNORM, usage,
                                     TextureType::TEXTURE_2D);
    auto entityIdTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32_INTEGER, usage,
                                          TextureType::TEXTURE_2D);
    auto depthTex = api->CreateTexture(1, width, height, 1, TextureFormat::DEPTH,
                                       TextureUsage::DEPTH_ATTACHMENT, TextureType::TEXTURE_2D);
    MRT mrt;
    mrt[0] = {hdrTex};
    mrt[1] = {entityIdTex};
    mRenderTarget = api->CreateRenderTarget(
        TargetBufferFlags::COLOR0 | TargetBufferFlags::COLOR1 | TargetBufferFlags::DEPTH, width,
        height, mrt, {depthTex}, {});

    mParams.viewport = {0, 0, width, height};
    mParams.flags.clearMask =
        TargetBufferFlags::COLOR0 | TargetBufferFlags::COLOR1 | TargetBufferFlags::DEPTH;
    mStaticMeshPipeline.rasterState.colorTargetCount = 2;
    mStaticMeshPipeline.rasterState.rasterizationSamples = 1;
}

void TestRenderPass::Draw() {
    mParams.clearColor = {0.3, 0.3, 0.3, 1.0};
    mParams.clearDepth = 1.0f;
    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();

    const auto& staticMeshDrawList = mDrawLists.StaticMeshDrawList;
    const auto& meshDrawList = mDrawLists.MeshDrawList;
    const auto& meshTransformMap = mDrawLists.MeshTransformMap;

    auto cube = RenderResource::Cube;
    uniformManager->UpdateScene();

    api->BeginRenderPass(mRenderTarget, mParams);
    uniformManager->Commit();  // TODO: should use the same command buffer as api->BeginRenderPass
    uniformManager->Bind();
    api->Draw(cube->GetMeshSource()->GetRenderPrimitive(), mStaticMeshPipeline);

    api->EndRenderPass();
}

void TestRenderPass::OnResize(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    // TODO: move this stuff to api
    api->DestroyRenderTarget(mRenderTarget);

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32G32B32A32_HDR, usage,
                                     TextureType::TEXTURE_2D);
    auto entityIdTex = api->CreateTexture(1, width, height, 1, TextureFormat::R32_INTEGER, usage,
                                          TextureType::TEXTURE_2D);
    auto depthTex = api->CreateTexture(1, width, height, 1, TextureFormat::DEPTH,
                                       TextureUsage::DEPTH_ATTACHMENT, TextureType::TEXTURE_2D);
    MRT mrt;
    mrt[0] = {hdrTex};
    mrt[1] = {entityIdTex};
    mRenderTarget = api->CreateRenderTarget(
        TargetBufferFlags::COLOR0 | TargetBufferFlags::COLOR1 | TargetBufferFlags::DEPTH, width,
        height, mrt, {depthTex}, {});

    mParams.viewport = {0, 0, width, height};
}

}  // namespace Ethereal
