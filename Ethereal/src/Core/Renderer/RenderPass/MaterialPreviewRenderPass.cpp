#include "MaterialPreviewRenderPass.h"
#include <Base/GlobalContext.h>

namespace Ethereal {
void MaterialPreviewRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();

    mObject = RenderResource::Cube;

    ShaderSource source;
    source[ShaderType::VERTEX] = SKYBOX_VERT;
    source[ShaderType::FRAGMENT] = SKYBOX_FRAG;
    mSkyboxPipeline.program = api->CreateProgram("SKYBOX", source);
    mSkyboxPipeline.rasterState.depthFunc = RasterState::DepthFunc::LE;

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R16G16B16A16_HDR, usage,
                                     TextureType::TEXTURE_2D);
    auto depthTex = api->CreateTexture(1, width, height, 1, TextureFormat::DEPTH,
                                       TextureUsage::DEPTH_ATTACHMENT, TextureType::TEXTURE_2D);
    MRT mrt;
    mrt[0] = {hdrTex};
    mRenderTarget = api->CreateRenderTarget(TargetBufferFlags::COLOR0 | TargetBufferFlags::DEPTH,
                                            width, height, mrt, {depthTex}, {});
}

void MaterialPreviewRenderPass::Draw() {
    if (!mPipeline.program) {
        ET_CORE_WARN("Material not set!");
        return;
    }

    mParams.clearColor = {0.1, 0.1, 0.1, 1.0};
    auto uniformManager = GlobalContext::GetUniformManager();
    auto api = GlobalContext::GetDriverApi();

    api->BeginRenderPass(mRenderTarget, mParams);

    // Draw Static Mesh

    // api->Draw(mObject->GetMeshSource()->GetRenderPrimitive(), mPipeline);

    // Draw Skybox
    uniformManager->Bind();
    api->Draw(RenderResource::Cube->GetMeshSource()->GetRenderPrimitive(), mSkyboxPipeline);

    api->EndRenderPass();
}

void MaterialPreviewRenderPass::OnResize(uint32_t width, uint32_t height) {
    if (mRenderTarget) {
        if (width == mRenderTarget->width && height == mRenderTarget->height) return;
    }

    auto api = GlobalContext::GetDriverApi();
    // TODO: move this stuff to api
    api->DestroyRenderTarget(mRenderTarget);

    auto usage = TextureUsage::COLOR_ATTACHMENT | TextureUsage::SAMPLEABLE;
    auto hdrTex = api->CreateTexture(1, width, height, 1, TextureFormat::R16G16B16A16_HDR, usage,
                                     TextureType::TEXTURE_2D);
    auto depthTex = api->CreateTexture(1, width, height, 1, TextureFormat::DEPTH,
                                       TextureUsage::DEPTH_ATTACHMENT, TextureType::TEXTURE_2D);
    MRT mrt;
    mrt[0] = {hdrTex};
    mRenderTarget = api->CreateRenderTarget(TargetBufferFlags::COLOR0 | TargetBufferFlags::DEPTH,
                                            width, height, mrt, {depthTex}, {});
}

void MaterialPreviewRenderPass::SetMaterial(MaterialCorePtr mat) {
    mPipeline.program = mat->GetProgram();
}

}  // namespace Ethereal
