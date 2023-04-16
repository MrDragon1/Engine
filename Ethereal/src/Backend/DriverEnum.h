#pragma once

#include "Utils/BitmaskEnum.h"

namespace Ethereal {
static constexpr size_t MAX_SUPPORTED_RENDER_TARGET_COUNT = 8u;
static constexpr size_t MAX_VERTEX_ATTRIBUTE_COUNT = 16u;

enum class BackendType : uint8_t {
    NONE = 0,
    OPENGL = 1,
};

enum class TextureUsage : uint8_t {
    NONE = 0x1 << 0,
    COLOR_ATTACHMENT = 0x1 << 1,
    DEPTH_ATTACHMENT = 0x1 << 2,
    SAMPLEABLE = 0x1 << 3,
    UPLOADABLE = 0x1 << 4,
    DEFAULT = SAMPLEABLE | UPLOADABLE,
};

enum class TextureFormat : uint16_t {
    R8G8B8_UNORM,
    R8G8B8_SRGB,
    R8G8B8A8_UNORM,
    R8G8B8A8_SRGB,
    R32G32_FLOAT,
    R32G32B32_FLOAT,
    R32G32B32A32_FLOAT,
    R32_INTEGER,
    DEPTH,
    R8,
    R16,
    R16G16B16A16_HDR,
    R16G16B16_HDR,
};

enum class TextureType : uint8_t {
    TEXTURE_2D,
    TEXTURE_2D_ARRAY,
    TEXTURE_3D,
    TEXTURE_CUBEMAP,
};

enum class PixelDataFormat : uint8_t {
    R,
    RI,
    RG,
    RGI,
    RGB,
    RGBI,
    RGBA,
    RGBAI,
    DEPTH,
    DEPTH_STENCIL,
    ALPHA,
};

enum class PixelDataType : uint8_t {
    BYTE,
    UBYTE,
    SHORT,
    USHORT,
    INT,
    UINT,
    FLOAT,
};

enum class BufferUsage : uint8_t {
    STATIC,
    DYNAMIC,
};

enum class ElementType : uint8_t {
    BYTE,
    BYTE2,
    BYTE3,
    BYTE4,
    UBYTE,
    UBYTE2,
    UBYTE3,
    UBYTE4,
    SHORT,
    SHORT2,
    SHORT3,
    SHORT4,
    USHORT,
    USHORT2,
    USHORT3,
    USHORT4,
    INT,
    INT2,
    INT3,
    INT4,
    UINT,
    UINT2,
    UINT3,
    UINT4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
};

static constexpr size_t ResolveElementTypeSize(ElementType type) noexcept {
    switch (type) {
        case ElementType::BYTE:
            return sizeof(int8_t);
        case ElementType::BYTE2:
            return sizeof(int8_t) * 2;
        case ElementType::BYTE3:
            return sizeof(int8_t) * 3;
        case ElementType::BYTE4:
            return sizeof(int8_t) * 4;
        case ElementType::UBYTE:
            return sizeof(uint8_t);
        case ElementType::UBYTE2:
            return sizeof(uint8_t) * 2;
        case ElementType::UBYTE3:
            return sizeof(uint8_t) * 3;
        case ElementType::UBYTE4:
            return sizeof(uint8_t) * 4;
        case ElementType::SHORT:
            return sizeof(int16_t);
        case ElementType::SHORT2:
            return sizeof(int16_t) * 2;
        case ElementType::SHORT3:
            return sizeof(int16_t) * 3;
        case ElementType::SHORT4:
            return sizeof(int16_t) * 4;
        case ElementType::USHORT:
            return sizeof(uint16_t);
        case ElementType::USHORT2:
            return sizeof(uint16_t) * 2;
        case ElementType::USHORT3:
            return sizeof(uint16_t) * 3;
        case ElementType::USHORT4:
            return sizeof(uint16_t) * 4;
        case ElementType::INT:
            return sizeof(int32_t);
        case ElementType::INT2:
            return sizeof(int32_t) * 2;
        case ElementType::INT3:
            return sizeof(int32_t) * 3;
        case ElementType::INT4:
            return sizeof(int32_t) * 4;
        case ElementType::UINT:
            return sizeof(uint32_t);
        case ElementType::UINT2:
            return sizeof(uint32_t) * 2;
        case ElementType::UINT3:
            return sizeof(uint32_t) * 3;
        case ElementType::UINT4:
            return sizeof(uint32_t) * 4;
        case ElementType::FLOAT:
            return sizeof(float);
        case ElementType::FLOAT2:
            return sizeof(float) * 2;
        case ElementType::FLOAT3:
            return sizeof(float) * 3;
        case ElementType::FLOAT4:
            return sizeof(float) * 4;
    }
}

struct Attribute {
    //! attribute is normalized (remapped between 0 and 1)
    static constexpr uint8_t FLAG_NORMALIZED = 0x1;
    //! attribute is an integer
    static constexpr uint8_t FLAG_INTEGER_TARGET = 0x2;
    static constexpr uint8_t BUFFER_UNUSED = 0xFF;

    uint32_t offset = 0;
    uint8_t stride = 0;
    uint8_t buffer = BUFFER_UNUSED;  // the buffer index of this attribute
    ElementType type = ElementType::BYTE;
    uint8_t flags = 0x0;
};

using AttributeArray = std::array<Attribute, MAX_VERTEX_ATTRIBUTE_COUNT>;

enum class BufferObjectBinding : uint8_t {
    VERTEX,
    UNIFORM,
    SHADER_STORAGE,
};

enum class PrimitiveType : uint8_t {
    POINTS = 0,
    LINES = 1,
    LINE_STRIP = 3,
    TRIANGLES = 4,
    TRIANGLE_STRIP = 5,
};

enum class ShaderType : uint8_t {
    VERTEX,
    FRAGMENT,
    GEOMETRY,
};

enum class SamplerWrapMode : uint8_t {
    CLAMP_TO_EDGE,
    REPEAT,
    MIRRORED_REPEAT,
};

enum class SamplerMinFilter : uint8_t {
    NEAREST = 0,
    LINEAR = 1,
    NEAREST_MIPMAP_NEAREST = 2,
    LINEAR_MIPMAP_NEAREST = 3,
    NEAREST_MIPMAP_LINEAR = 4,
    LINEAR_MIPMAP_LINEAR = 5,
};

//! Sampler magnification filter
enum class SamplerMagFilter : uint8_t {
    NEAREST = 0,
    LINEAR = 1,
};

//! Sampler compare mode
enum class SamplerCompareMode : uint8_t {
    NONE = 0,
    COMPARE_TO_TEXTURE = 1,
};

//! comparison function for the depth / stencil sampler
enum class SamplerCompareFunc : uint8_t {
    LE = 0,
    GE,
    L,
    G,
    E,
    NE,
    A,
    N,
};

struct SamplerParams {
    static SamplerParams Default() noexcept {
        SamplerParams params;
        params.filterMag = SamplerMagFilter::LINEAR;
        params.filterMin = SamplerMinFilter::LINEAR_MIPMAP_LINEAR;
        params.wrapS = SamplerWrapMode::REPEAT;
        params.wrapT = SamplerWrapMode::REPEAT;
        params.wrapR = SamplerWrapMode::REPEAT;
        params.anisotropyLog2 = 0;
        params.compareMode = SamplerCompareMode::NONE;
        params.compareFunc = SamplerCompareFunc::LE;
        return params;
    }

    SamplerMagFilter filterMag : 1;
    SamplerMinFilter filterMin : 3;
    SamplerWrapMode wrapS : 2;
    SamplerWrapMode wrapT : 2;
    SamplerWrapMode wrapR : 2;
    uint8_t anisotropyLog2 : 3;
    SamplerCompareMode compareMode : 1;
    SamplerCompareFunc compareFunc : 3;
};

struct RenderPassParams {
    Vector4 clearColor = {0, 0, 0, 1};
};

struct RasterState {
    using DepthFunc = SamplerCompareFunc;
    bool EnableDepthWrite = true;
    bool EnableDepthTest = true;
    DepthFunc depthFunc = DepthFunc::L;
};

enum class TargetBufferFlags : uint32_t {
    NONE = 0x0u,
    COLOR0 = 0x00000001u,
    COLOR1 = 0x00000002u,
    COLOR2 = 0x00000004u,
    COLOR3 = 0x00000008u,
    COLOR4 = 0x00000010u,
    COLOR5 = 0x00000020u,
    COLOR6 = 0x00000040u,
    COLOR7 = 0x00000080u,

    COLOR = COLOR0,
    COLOR_ALL = COLOR0 | COLOR1 | COLOR2 | COLOR3 | COLOR4 | COLOR5 | COLOR6 | COLOR7,
    DEPTH = 0x10000000u,
    STENCIL = 0x20000000u,
    DEPTH_AND_STENCIL = DEPTH | STENCIL,
    ALL = COLOR_ALL | DEPTH | STENCIL
};

inline constexpr TargetBufferFlags getTargetBufferFlagsAt(size_t index) noexcept {
    if (index == 0u) return TargetBufferFlags::COLOR0;
    if (index == 1u) return TargetBufferFlags::COLOR1;
    if (index == 2u) return TargetBufferFlags::COLOR2;
    if (index == 3u) return TargetBufferFlags::COLOR3;
    if (index == 4u) return TargetBufferFlags::COLOR4;
    if (index == 5u) return TargetBufferFlags::COLOR5;
    if (index == 6u) return TargetBufferFlags::COLOR6;
    if (index == 7u) return TargetBufferFlags::COLOR7;
    if (index == 8u) return TargetBufferFlags::DEPTH;
    if (index == 9u) return TargetBufferFlags::STENCIL;
    return TargetBufferFlags::NONE;
}

enum class UniformType : uint8_t {
    INT,
    INT2,
    INT3,
    INT4,
    UINT,
    UINT2,
    UINT3,
    UINT4,
    BOOL,
    BOOL2,
    BOOL3,
    BOOL4,
    FLOAT,
    FLOAT2,
    FLOAT3,
    FLOAT4,
    MAT3,
    MAT4,
    STRUCT,
};

enum class Precision : uint8_t {
    LOW,
    MEDIUM,
    HIGH,
    DEFAULT,
};
}  // namespace Ethereal

template <>
struct Utils::EnableBitMaskOperators<Ethereal::TextureUsage> : public std::true_type {};
template <>
struct Utils::EnableBitMaskOperators<Ethereal::TargetBufferFlags> : public std::true_type {};