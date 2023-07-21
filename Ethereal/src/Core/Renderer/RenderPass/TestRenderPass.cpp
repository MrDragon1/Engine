#include "TestRenderPass.h"
#include <Base/GlobalContext.h>

namespace Ethereal {

void TestRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    ShaderSource source;
    source[ShaderType::VERTEX] = TEST_VERT;
    source[ShaderType::FRAGMENT] = TEST_FRAG;
    mStaticMeshPipeline.program = api->CreateProgram("TEST", source);

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R16G16B16A16_HDR, usage,
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
}

void TestRenderPass::Draw() {
    mParams.clearColor = {0.1, 1.0, 0.1, 1.0};
    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();

    const auto& staticMeshDrawList = mDrawLists.StaticMeshDrawList;
    const auto& meshDrawList = mDrawLists.MeshDrawList;
    const auto& meshTransformMap = mDrawLists.MeshTransformMap;

    auto cube = RenderResource::Cube;

    api->BeginRenderPass(mRenderTarget, mParams);

    api->Draw(cube->GetMeshSource()->GetRenderPrimitive(), mStaticMeshPipeline);

    api->EndRenderPass();
}

void TestRenderPass::OnResize(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();
    // TODO: move this stuff to api
    api->DestroyRenderTarget(mRenderTarget);

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R16G16B16A16_HDR, usage,
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