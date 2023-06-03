#include "MaterialCore.h"

namespace Ethereal {

void MaterialCore::Compile() {
    VariableBlock::sBinding = 0;
    mShaderContext = ShaderContextPtr::Create();
    ShaderGenerator& shaderGen = mShaderContext->GetShaderGenerator();
    mShaderContext->mShaderGraph = mDocument->GenerateShaderGraph(mShaderContext);

    mShaderContext->PushScope(mShaderContext->mShaderGraph->GetName());

    mShaderContext->mShader = ShaderPtr::Create("shader");
    auto shader = mShaderContext->GetShader();

    ShaderStagePtr vs = shader->CreateStage(Stage::VERTEX);
    VariableBlockPtr vsInputs = vs->CreateInputBlock(ShaderBuildInVariable::INPUT);
    vsInputs->Add(nullptr, "float3", ShaderBuildInVariable::IN_POSITION);
    vsInputs->Add(nullptr, "float3", ShaderBuildInVariable::IN_NORMAL);
    VariableBlockPtr vsPublicUniforms = vs->CreateUniformBlock(ShaderBuildInVariable::VSPUBUNIFORM);
    vsPublicUniforms->Add(nullptr, "matrix4", ShaderBuildInVariable::MODEL_MATRIX);
    vsPublicUniforms->Add(nullptr, "matrix4", ShaderBuildInVariable::VIEW_MATRIX);
    vsPublicUniforms->Add(nullptr, "matrix4", ShaderBuildInVariable::PROJ_MATRIX);

    ShaderStagePtr ps = shader->CreateStage(Stage::PIXEL);
    VariableBlockPtr psPublicUniforms = ps->CreateUniformBlock(ShaderBuildInVariable::PSPUBUNIFORM);
    VariableBlockPtr psPrivateUniforms = ps->CreateUniformBlock(ShaderBuildInVariable::PRVUNIFORM);
    psPrivateUniforms->Add(nullptr, "integer", ShaderBuildInVariable::NUM_ACTIVE_LIGHT_SOURCES);

    VariableBlockPtr psOutputs = ps->CreateOutputBlock(ShaderBuildInVariable::OUTPUT);

    VariableBlockPtr lightData = ps->CreateUniformBlock(ShaderBuildInVariable::LIGHT_DATA,
                                                        ShaderBuildInVariable::LIGHT_DATA_INSTANCE);

    auto shaderGraph = mShaderContext->GetShaderGraph();

    for (auto& [name, inputSocket] : shaderGraph->GetInputSockets()) {
        psPublicUniforms->Add(inputSocket);
    }

    auto outputSocket = shaderGraph->GetOutputSockets().begin()->second;
    ShaderPortPtr output = psOutputs->Add(nullptr, "color4", outputSocket->GetName());
    output->SetVariable(outputSocket->GetVariable(mShaderContext->GetScope()));

    // connect between vs & ps
    shaderGen.AddConnectorBlock(ShaderBuildInVariable::VERTEXDATA, "vd", vs, ps);

    shaderGen.CreateVariables(shaderGraph, mShaderContext, shader);

    shaderGen.EmitVertexStage(shaderGraph, mShaderContext, vs);
    shaderGen.EmitPixelStage(shaderGraph, mShaderContext, ps);

    shader->Compile();
    mIsCompile = true;
}

}  // namespace Ethereal
