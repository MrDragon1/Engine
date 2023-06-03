#include "MaterialPreviewRenderPass.h"
#include <Base/GlobalContext.h>

namespace Ethereal {
void MaterialPreviewRenderPass::Init(uint32_t width, uint32_t height) {
    auto api = GlobalContext::GetDriverApi();

    mObject = RenderResource::Sphere;

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
    auto& param = Project::GetConfigManager().sUniformManagerConfig.PreivewCameraParam;
    Matrix4 model = Matrix4::IDENTITY;

    auto& vsblocks = mMaterial->GetUniforms(Stage::VERTEX);
    auto& psblocks = mMaterial->GetUniforms(Stage::PIXEL);
    auto& vsblock = vsblocks[ShaderBuildInVariable::VSPUBUNIFORM];
    vsblock->GetVariable(ShaderBuildInVariable::MODEL_MATRIX)->GetValue()->SetData<Matrix4>(model);
    vsblock->GetVariable(ShaderBuildInVariable::VIEW_MATRIX)
        ->GetValue()
        ->SetData<Matrix4>(param.ViewMatrix);
    vsblock->GetVariable(ShaderBuildInVariable::PROJ_MATRIX)
        ->GetValue()
        ->SetData<Matrix4>(param.ProjectionMatrix);

    vsblock->GetVariable(ShaderBuildInVariable::MODEL_INVERSE_TRANSPOSE_MATRIX)
        ->GetValue()
        ->SetData<Matrix4>(Math::Transpose(Math::Inverse(model)));

    for (auto& [name, var] : vsblock->GetRawVariables()) {
        if (!var->GetValue()) ET_CORE_WARN("uniform {0} has no value", name);
        api->BindUniform(mPipeline.program, var->GetVariable(mMaterial->GetContext()->GetScope()),
                         var->GetValue());
    }

    for (auto& [name, var] : psblocks[ShaderBuildInVariable::PSPUBUNIFORM]->GetRawVariables()) {
        if (!var->GetValue()) ET_CORE_WARN("uniform {0} has no value", name);
        api->BindUniform(mPipeline.program, var->GetVariable(mMaterial->GetContext()->GetScope()),
                         var->GetValue());
    }

    ValueBasePtr value = ValueBase::CreateValueWithType("float3");
    value->SetData<Vector3>(param.CameraPosition);
    api->BindUniform(mPipeline.program, "view_subtract_in1", value);

    api->Draw(mObject->GetMeshSource()->GetRenderPrimitive(), mPipeline);

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
    mMaterial = mat;
    mPipeline.program = mat->GetProgram();
}

}  // namespace Ethereal