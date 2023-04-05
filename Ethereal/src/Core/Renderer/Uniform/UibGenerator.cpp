#include "UibGenerator.h"

#include "Platform/Windows/Backend/UniformBuffer.h"
namespace Ethereal {
using namespace Backend;
BufferInterfaceBlock const& UibGenerator::GetCameraUib() noexcept {
    using Type = BufferInterfaceBlock::Type;

    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(CameraUib::_name)
                                                .Add({
                                                    {"ViewProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"InverseViewProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"ProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"InverseProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"ViewMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"InverseViewMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"FarPlane", 0, Type::FLOAT, Precision::HIGH},
                                                    {"NearPlane", 0, Type::FLOAT, Precision::HIGH},

                                                    {"padding", 0, Type::FLOAT2, Precision::HIGH},
                                                })
                                                .Build();

    return uib;
}

BufferInterfaceBlock const& UibGenerator::GetShadowUib() noexcept {
    using Type = BufferInterfaceBlock::Type;

    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(ShadowUib::_name)
                                                .Add({
                                                    {"CascadeSplits", 16, Type::FLOAT4, Precision::HIGH},
                                                    {"DirLightMatrices", 16, Type::MAT4, Precision::HIGH},
                                                    {"CascadeCount", 0, Type::UINT, Precision::HIGH},

                                                    {"padding", 0, Type::FLOAT3, Precision::HIGH},
                                                })
                                                .Build();

    return uib;
}

BufferInterfaceBlock const& UibGenerator::GetSceneUib() noexcept {
    using Type = BufferInterfaceBlock::Type;
    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(SceneUib::_name)
                                                .Add({
                                                    //{"DirLight", 0, Type::STRUCT, {}, "DirectionalLight", sizeof(SceneUib::DirectionalLight), {}},
                                                    {"CameraPosition", 0, Type::FLOAT3, Precision::HIGH},
                                                    {"EnvironmentMapIntensity", 0, Type::FLOAT, Precision::HIGH},
                                                    {"ScissorNormalized", 0, Type::FLOAT4, Precision::HIGH},

                                                    // Fog
                                                    {"FogColor", 0, Type::FLOAT4, Precision::HIGH},
                                                    {"FogStart", 0, Type::FLOAT, Precision::HIGH},
                                                    {"FogDensity", 0, Type::FLOAT, Precision::HIGH},
                                                    {"FogHeight", 0, Type::FLOAT, Precision::HIGH},
                                                    {"FogHeightFalloff", 0, Type::FLOAT, Precision::HIGH},
                                                    {"FogScatteringStart", 0, Type::FLOAT, Precision::HIGH},
                                                    {"FogScatteringSize", 0, Type::FLOAT, Precision::HIGH},
                                                    {"FogEnbale", 0, Type::BOOL, Precision::HIGH},
                                                    {"FogFromIBL", 0, Type::BOOL, Precision::HIGH},
                                                })
                                                .Build();
    return uib;
}

BufferInterfaceBlock const& UibGenerator::GetEditorUib() noexcept {
    using Type = BufferInterfaceBlock::Type;
    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(EditorUib::_name)
                                                .Add({
                                                    {"EntityID", 0, Type::UINT, Precision::HIGH},

                                                    {"padding", 0, Type::FLOAT3, Precision::HIGH},
                                                })
                                                .Build();
    return uib;
}

}  // namespace Ethereal