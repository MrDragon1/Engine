#include "UibGenerator.h"

#include "Platform/Windows/Backend/UniformBuffer.h"
namespace Ethereal {
BufferInterfaceBlock const& UibGenerator::GetViewUib() noexcept {
    using Type = BufferInterfaceBlock::Type;

    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(ViewUib::_name)
                                                .Add({
                                                    {"CameraPosition", 0, Type::FLOAT3, Precision::HIGH},
                                                    {"EnvironmentMapIntensity", 0, Type::FLOAT, Precision::HIGH},
                                                    {"ScissorNormalized", 0, Type::FLOAT4, Precision::HIGH},

                                                    // Camera
                                                    {"ViewProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"InverseViewProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"ProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"InverseProjectionMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"ViewMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"InverseViewMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"FarPlane", 0, Type::FLOAT, Precision::HIGH},
                                                    {"NearPlane", 0, Type::FLOAT, Precision::HIGH},
                                                    {"EntityID", 0, Type::UINT, Precision::HIGH},
                                                    {"padding", 0, Type::FLOAT, Precision::HIGH},
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

BufferInterfaceBlock const& UibGenerator::GetRenderPrimitiveUib() noexcept {
    using Type = BufferInterfaceBlock::Type;
    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(RenderPrimitiveUib::_name)
                                                .Add({
                                                    //{"DirLight", 0, Type::STRUCT, {}, "DirectionalLight", sizeof(SceneUib::DirectionalLight), {}},
                                                    {"ModelMatrix", 0, Type::MAT4, Precision::HIGH},
                                                    {"Albedo", 0, Type::FLOAT4, Precision::HIGH},
                                                    {"Metallic", 0, Type::FLOAT, Precision::HIGH},
                                                    {"Roughness", 0, Type::FLOAT, Precision::HIGH},
                                                    {"Occlusion", 0, Type::FLOAT, Precision::HIGH},
                                                    {"Emisstion", 0, Type::FLOAT, Precision::HIGH},
                                                    {"UseMap", 0, Type::UINT, Precision::HIGH},
                                                })
                                                .Build();
    return uib;
}

BufferInterfaceBlock const& UibGenerator::GetRenderPrimitiveBoneUib() noexcept {
    using Type = BufferInterfaceBlock::Type;
    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(RenderPrimitiveBoneUib::_name)
                                                .Add({
                                                    {"BoneTransform", 100, Type::MAT4, Precision::HIGH},
                                                })
                                                .Build();
    return uib;
}

BufferInterfaceBlock const& UibGenerator::GetLightUib() noexcept {
    using Type = BufferInterfaceBlock::Type;
    static BufferInterfaceBlock const uib = BufferInterfaceBlock::Builder()
                                                .Name(LightUib::_name)
                                                .Add({
                                                    {"Direction", 0, Type::FLOAT3, Precision::HIGH},
                                                    {"ShadowAmount", 0, Type::FLOAT, Precision::HIGH},
                                                    {"Color", 0, Type::FLOAT3, Precision::HIGH},
                                                    {"Intensity", 0, Type::FLOAT, Precision::HIGH},
                                                })
                                                .Build();
    return uib;
}

}  // namespace Ethereal