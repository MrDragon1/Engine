#include "MaterialCore.h"

namespace Ethereal {

void MaterialCore::Compile() {
    VariableBlock::sBinding = 0;
    mShaderContext = ShaderContextPtr::Create();
    ShaderGenerator& shaderGen = mShaderContext->GetShaderGenerator();
    mShaderContext->mShaderGraph = mDocument->GenerateShaderGraph();

    mShaderContext->PushScope(mShaderContext->mShaderGraph->GetName());

    auto shaderGraph = mShaderContext->GetShaderGraph();
    mShaderContext->mShader = ShaderPtr::Create("shader", shaderGraph);
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
    VariableBlockPtr psOutputs = ps->CreateOutputBlock(ShaderBuildInVariable::OUTPUT);

    for (auto& [name, inputSocket] : shaderGraph->GetInputSockets()) {
        psPublicUniforms->Add(inputSocket);
    }

    auto outputSocket = shaderGraph->GetOutputSockets().begin()->second;
    ShaderPortPtr output = psOutputs->Add(nullptr, "color4", outputSocket->GetName());
    output->SetVariable(outputSocket->GetVariable(mShaderContext->GetScope()));

    // connect between vs & ps
    shaderGen.AddConnectorBlock("vertexData", "vd", vs, ps);
    shaderGen.AddConnectorVariable("vertexData", "float3", ShaderBuildInVariable::POSITION_WORLD,
                                   vs, ps);

    shaderGen.CreateVariables(shaderGraph, mShaderContext, shader);

    shaderGen.EmitVertexStage(shaderGraph, mShaderContext, vs);
    shaderGen.EmitPixelStage(shaderGraph, mShaderContext, ps);

    shader->Compile();
    mIsCompile = true;
}

}  // namespace Ethereal
