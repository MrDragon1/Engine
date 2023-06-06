#include "LightManager.h"
#include "Base/GlobalContext.h"
namespace Ethereal {

void LightManager::Bind(MaterialCorePtr mat) {
    auto api = GlobalContext::GetDriverApi();
    auto program = mat->GetProgram();
    int index = 0;
    for (const auto& light : mDirectionalLights) {
        string prefix =
            ShaderBuildInVariable::LIGHT_DATA_INSTANCE + "[" + std::to_string(index++) + "].";
        api->BindUniform(program, prefix + "type", ValueBase::CreateValue<int>(0));
        api->BindUniform(program, prefix + "direction",
                         ValueBase::CreateValue<Vector3>(light.mDirection));
        api->BindUniform(program, prefix + "intensity",
                         ValueBase::CreateValue<float>(light.mIntensity));
        api->BindUniform(program, prefix + "color", ValueBase::CreateValue<Color3>(light.mColor));
    }
    for (const auto& light : mPointLights) {
        string prefix =
            ShaderBuildInVariable::LIGHT_DATA_INSTANCE + "[" + std::to_string(index++) + "].";
        api->BindUniform(program, prefix + "type", ValueBase::CreateValue<int>(1));
        api->BindUniform(program, prefix + "direction",
                         ValueBase::CreateValue<Vector3>(light.mPosition));
        api->BindUniform(program, prefix + "intensity",
                         ValueBase::CreateValue<float>(light.mIntensity));
        api->BindUniform(program, prefix + "color", ValueBase::CreateValue<Color3>(light.mColor));
    }

    auto& psblocks = mat->GetUniforms(Stage::PIXEL);
    auto& psPrivateblock = psblocks[ShaderBuildInVariable::PRVUNIFORM];
    psPrivateblock->GetVariable(ShaderBuildInVariable::NUM_ACTIVE_LIGHT_SOURCES)
        ->GetValue()
        ->SetData(index);
}

void LightManager::Clear() {
    mDirectionalLights.clear();
    mPointLights.clear();
}

}  // namespace Ethereal
