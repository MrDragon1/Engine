#pragma once

#include "Utils/BitmaskEnum.h"

namespace Ethereal {
namespace Backend {
static constexpr size_t MAX_COLOR_ATTACHMENT = 8u;
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
    RED,
    R16G16B16A16_HDR,
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
    UINT,
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
        case ElementType::UINT:
            return sizeof(uint32_t);
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
    COMPUTE,
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
    SamplerMagFilter filterMag : 1;
    SamplerMinFilter filterMin : 3;
    SamplerWrapMode wrapS : 2;
    SamplerWrapMode wrapT : 2;
    SamplerWrapMode wrapR : 2;
    uint8_t anisotropyLog2 : 3;
    SamplerCompareMode compareMode : 1;
    SamplerCompareFunc compareFunc : 3;
};
}  // namespace Backend
}  // namespace Ethereal

template <>
struct Utils::EnableBitMaskOperators<Ethereal::Backend::TextureUsage> : public std::true_type {};
