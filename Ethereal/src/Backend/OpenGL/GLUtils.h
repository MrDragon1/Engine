#pragma once
#include <glad/glad.h>
namespace Ethereal {

namespace Backend {

namespace GLUtils {

static GLenum ResolveTextureFormat(const TextureFormat& format) {
    // m_InternalFormat: https://www.khronos.org/opengl/wiki/Texture_Storage

    // m_DataFormat: https://docs.gl/gl4/glTexSubImage2D
    // Only: GL_RED, GL_RG, GL_RGB, GL_BGR, GL_RGBA, GL_BGRA, GL_DEPTH_COMPONENT, and GL_STENCIL_INDEX.
    switch (format) {
        case TextureFormat::R8G8B8_UNORM:
            return GL_RGB8;
        case TextureFormat::R8G8B8_SRGB:
            return GL_SRGB8;
        case TextureFormat::R8G8B8A8_UNORM:
            return GL_RGBA8;
        case TextureFormat::R8G8B8A8_SRGB:
            return GL_SRGB8_ALPHA8;
        case TextureFormat::R32G32_FLOAT:
            return GL_RG32F;
        case TextureFormat::R32G32B32_FLOAT:
            return GL_RGB32F;
        case TextureFormat::R32G32B32A32_FLOAT:
            return GL_RGBA32F;
        case TextureFormat::R32_INTEGER:
            return GL_R32I;
        case TextureFormat::DEPTH:
            return GL_DEPTH_COMPONENT32F;
        case TextureFormat::R8:
            return GL_R8;
        case TextureFormat::R16:
            return GL_R16;
        case TextureFormat::R16G16B16A16_HDR:
            return GL_RGBA16F;  // Convert to RGBA
        case TextureFormat::R16G16B16_HDR:
            return GL_RGB16F;  // Convert to RGBA
        default:
            ET_CORE_ASSERT("Invalid Texture Format!");
            return GL_NONE;
    }
}
static GLenum ResolveTextureType(const TextureType& type) {
    switch (type) {
        case TextureType::TEXTURE_2D:
            return GL_TEXTURE_2D;

        case TextureType::TEXTURE_2D_ARRAY:
            return GL_TEXTURE_2D_ARRAY;

        case TextureType::TEXTURE_3D:
            return GL_TEXTURE_3D;

        case TextureType::TEXTURE_CUBEMAP:
            return GL_TEXTURE_CUBE_MAP;

        default:
            ET_CORE_ASSERT("Invalid Texture Type!");
            return GL_NONE;
    }
}
static GLenum ResolvePixelDataFormat(const PixelDataFormat& format) {
    switch (format) {
        case PixelDataFormat::R:
            return GL_RED;
        case PixelDataFormat::RI:
            return GL_RED_INTEGER;
        case PixelDataFormat::RG:
            return GL_RG;
        case PixelDataFormat::RGI:
            return GL_RG_INTEGER;
        case PixelDataFormat::RGB:
            return GL_RGB;
        case PixelDataFormat::RGBI:
            return GL_RGB_INTEGER;
        case PixelDataFormat::RGBA:
            return GL_RGBA;
        case PixelDataFormat::RGBAI:
            return GL_RGBA_INTEGER;
        case PixelDataFormat::DEPTH:
            return GL_DEPTH_COMPONENT;
        case PixelDataFormat::DEPTH_STENCIL:
            return GL_DEPTH_STENCIL;
        case PixelDataFormat::ALPHA:
            return GL_ALPHA;
        default:
            ET_CORE_ASSERT("Invalid PixelDataFormat!")
            return GL_NONE;
    }
}
static GLenum ResolvePixelDataType(const PixelDataType& type) {
    switch (type) {
        case PixelDataType::BYTE:
            return GL_BYTE;
        case PixelDataType::UBYTE:
            return GL_UNSIGNED_BYTE;
        case PixelDataType::SHORT:
            return GL_SHORT;
        case PixelDataType::USHORT:
            return GL_UNSIGNED_SHORT;
        case PixelDataType::INT:
            return GL_INT;
        case PixelDataType::UINT:
            return GL_UNSIGNED_INT;
        case PixelDataType::FLOAT:
            return GL_FLOAT;
        default:
            ET_CORE_ASSERT("Invalid PixelData Type!")
            return GL_NONE;
    }
}
static GLenum ResolveBufferObjectBinding(const BufferObjectBinding& bindingType) {
    switch (bindingType) {
        case BufferObjectBinding::VERTEX:
            return GL_ARRAY_BUFFER;
        case BufferObjectBinding::UNIFORM:
            return GL_UNIFORM_BUFFER;
        case BufferObjectBinding::SHADER_STORAGE:
            return GL_SHADER_STORAGE_BUFFER;
        default:
            ET_CORE_ASSERT("Invalid BufferObjectBinding!")
            return GL_NONE;
    }
}
static GLenum ResolveBufferUsage(const BufferUsage& usage) {
    switch (usage) {
        case BufferUsage::STATIC:
            return GL_STATIC_DRAW;
        default:
            return GL_DYNAMIC_DRAW;
    }
}
static GLenum ResolveElementType(const ElementType& type) {
    using ElementType = ElementType;
    switch (type) {
        case ElementType::BYTE:
        case ElementType::BYTE2:
        case ElementType::BYTE3:
        case ElementType::BYTE4:
            return GL_BYTE;
        case ElementType::UBYTE:
        case ElementType::UBYTE2:
        case ElementType::UBYTE3:
        case ElementType::UBYTE4:
            return GL_UNSIGNED_BYTE;
        case ElementType::SHORT:
        case ElementType::SHORT2:
        case ElementType::SHORT3:
        case ElementType::SHORT4:
            return GL_SHORT;
        case ElementType::USHORT:
        case ElementType::USHORT2:
        case ElementType::USHORT3:
        case ElementType::USHORT4:
            return GL_UNSIGNED_SHORT;
        case ElementType::INT:
            return GL_INT;
        case ElementType::UINT:
            return GL_UNSIGNED_INT;
        case ElementType::FLOAT:
        case ElementType::FLOAT2:
        case ElementType::FLOAT3:
        case ElementType::FLOAT4:
            return GL_FLOAT;
        default:
            ET_CORE_ASSERT("Invalid ElementType!")
            return GL_NONE;
    }
}
static GLuint ResolveElementTypeCount(const ElementType& type) {
    using ElementType = ElementType;
    switch (type) {
        case ElementType::BYTE:
        case ElementType::UBYTE:
        case ElementType::SHORT:
        case ElementType::USHORT:
        case ElementType::INT:
        case ElementType::UINT:
        case ElementType::FLOAT:
            return 1;
        case ElementType::FLOAT2:
        case ElementType::BYTE2:
        case ElementType::UBYTE2:
        case ElementType::SHORT2:
        case ElementType::USHORT2:
            return 2;
        case ElementType::FLOAT3:
        case ElementType::BYTE3:
        case ElementType::UBYTE3:
        case ElementType::SHORT3:
        case ElementType::USHORT3:
            return 3;
        case ElementType::FLOAT4:
        case ElementType::BYTE4:
        case ElementType::UBYTE4:
        case ElementType::SHORT4:
        case ElementType::USHORT4:
            return 4;
        default:
            ET_CORE_ASSERT("Invalid ElementType!")
            return 0;
    }
}
static GLenum ResolveNormalize(bool normalized) { return GLboolean(normalized ? GL_TRUE : GL_FALSE); }
static GLenum ResolveShaderType(const ShaderType& type) {
    switch (type) {
        case ShaderType::VERTEX:
            return GL_VERTEX_SHADER;
        case ShaderType::FRAGMENT:
            return GL_FRAGMENT_SHADER;
        case ShaderType::GEOMETRY:
            return GL_GEOMETRY_SHADER;
        default:
            ET_CORE_ASSERT("Invalide ShaderType!")
            return GL_NONE;
    }
}
static GLenum ResolveSamplerMinFilter(const SamplerMinFilter& filter) {
    switch (filter) {
        case SamplerMinFilter::NEAREST:
            return GL_NEAREST;
        case SamplerMinFilter::LINEAR:
            return GL_LINEAR;
        case SamplerMinFilter::NEAREST_MIPMAP_NEAREST:
            return GL_NEAREST_MIPMAP_NEAREST;
        case SamplerMinFilter::LINEAR_MIPMAP_NEAREST:
            return GL_LINEAR_MIPMAP_NEAREST;
        case SamplerMinFilter::NEAREST_MIPMAP_LINEAR:
            return GL_NEAREST_MIPMAP_LINEAR;
        case SamplerMinFilter::LINEAR_MIPMAP_LINEAR:
            return GL_LINEAR_MIPMAP_LINEAR;
        default:
            ET_CORE_ASSERT("Invalid SamplerMinFilter!")
            return GL_NONE;
    }
}
static GLenum ResolveSamplerMagFilter(const SamplerMagFilter& filter) {
    switch (filter) {
        case SamplerMagFilter::NEAREST:
            return GL_NEAREST;
        case SamplerMagFilter::LINEAR:
            return GL_LINEAR;
        default:
            ET_CORE_ASSERT("Invalid SamplerMagFilter!")
            return GL_NONE;
    }
}
static GLenum ResolveSamplerWrapMode(const SamplerWrapMode& mode) {
    switch (mode) {
        case SamplerWrapMode::CLAMP_TO_EDGE:
            return GL_CLAMP_TO_EDGE;
        case SamplerWrapMode::MIRRORED_REPEAT:
            return GL_MIRRORED_REPEAT;
        case SamplerWrapMode::REPEAT:
            return GL_REPEAT;
        default:
            ET_CORE_ASSERT("Invalide SamplerWrapMode!")
            return GL_NONE;
    }
}
static GLenum ResolveSamplerCompareMode(const SamplerCompareMode& mode) {
    return mode == SamplerCompareMode::NONE ? GL_NONE : GL_COMPARE_REF_TO_TEXTURE;
}
static GLenum ResolveSamplerCompareFunc(const SamplerCompareFunc& func) noexcept {
    switch (func) {
        case SamplerCompareFunc::LE:
            return GL_LEQUAL;
        case SamplerCompareFunc::GE:
            return GL_GEQUAL;
        case SamplerCompareFunc::L:
            return GL_LESS;
        case SamplerCompareFunc::G:
            return GL_GREATER;
        case SamplerCompareFunc::E:
            return GL_EQUAL;
        case SamplerCompareFunc::NE:
            return GL_NOTEQUAL;
        case SamplerCompareFunc::A:
            return GL_ALWAYS;
        case SamplerCompareFunc::N:
            return GL_NEVER;
        default:
            ET_CORE_ASSERT("Invalid SamplerComapreFunc!")
            return GL_NONE;
    }
}
static GLenum ResolveDepthFunc(const SamplerCompareFunc& func) { return ResolveSamplerCompareFunc(func); }
}  // namespace GLUtils
}  // namespace Backend
}  // namespace Ethereal