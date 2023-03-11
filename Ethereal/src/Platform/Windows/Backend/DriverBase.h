#pragma once

#include "Base/Ref.h"
#include "Core/Renderer/Buffer.h"
#include "DriverEnums.h"
namespace Ethereal {
namespace Backend {
struct Texture;
struct SamplerGroup;
struct VertexBuffer;
struct IndexBuffer;
struct VertexArray;
struct UniformBuffer;
struct RenderPrimitive;
struct Program;
struct RenderTarget;
struct BufferObject;

using TextureHandle = Ref<Texture>;
using SamplerGroupHandle = Ref<SamplerGroup>;
using BufferObjectHandle = Ref<BufferObject>;
using VertexBufferHandle = Ref<VertexBuffer>;
using IndexBufferHandle = Ref<IndexBuffer>;
using VertexArrayHandle = Ref<VertexArray>;
using UniformBufferHandle = Ref<UniformBuffer>;
using RenderPrimitiveHandle = Ref<RenderPrimitive>;
using ProgramHandle = Ref<Program>;
using RenderTargetHandle = Ref<RenderTarget>;

struct Texture : public RefCounted {
    uint32_t width;
    uint32_t height;
    uint32_t depth;
    uint32_t levels;

    TextureFormat format;
    TextureUsage usage;
    TextureType type;

    Texture(uint32_t width, uint32_t height, uint32_t depth, uint32_t levels, TextureFormat format, TextureUsage usage, TextureType type)
        : width(width), height(height), depth(depth), levels(levels), format(format), usage(usage), type(type) {}
};

struct SamplerDescriptor {
    TextureHandle texture;
    SamplerParams params;
    uint8_t binding;
};
using SamplerGroupDescriptor = std::vector<SamplerDescriptor>;

struct SamplerGroup : public RefCounted {
    SamplerGroup() = default;
};

struct BufferObject : public RefCounted {
    uint32_t byteCount{};
    BufferObject(uint32_t count = 0) : byteCount(count){};
};

struct VertexBuffer : public RefCounted {
    AttributeArray attributes{};
    uint32_t vertexCount{};
    uint8_t attributeCount{};
    uint8_t bufferCount{};

    VertexBuffer(AttributeArray att, uint32_t verc, uint8_t attc, uint8_t buffer)
        : attributes(att), vertexCount(verc), attributeCount(attc), bufferCount(buffer){};
};

struct IndexBuffer : public RefCounted {
    uint32_t count : 27;
    uint32_t elementSize : 5;
    IndexBuffer() noexcept : count{}, elementSize{} {}
    IndexBuffer(uint8_t elementSize, uint32_t indexCount) noexcept : count(indexCount), elementSize(elementSize) {
        // we could almost store elementSize on 4 bits because it's never > 16 and never 0
        ET_CORE_ASSERT(elementSize > 0 && elementSize <= 16);
        ET_CORE_ASSERT(indexCount < (1u << 27));
    }
};

struct UniformBuffer : public RefCounted {};

struct RenderPrimitive : public RefCounted {
    uint32_t offset{};
    uint32_t minIndex{};
    uint32_t maxIndex{};
    uint32_t count{};
    uint32_t maxVertexCount{};
    PrimitiveType type = PrimitiveType::TRIANGLES;
    Ref<VertexBuffer> vertexBuffer = {};
    Ref<IndexBuffer> indexBuffer = {};
};

static constexpr size_t SHADER_TYPE_COUNT = 3;
using ShaderBlob = std::vector<unsigned char>;
using ShaderSource = std::unordered_map<ShaderType, ShaderBlob>;
struct Program : public RefCounted {
    std::string name;
    Program(std::string_view name) : name(name) {}
    Program() = default;
};

struct TargetBufferInfo {
    // texture to be used as render target
    TextureHandle handle;

    // level to be used
    uint8_t level = 0;

    // for cubemaps and 3D textures. See TextureCubemapFace for the face->layer mapping
    uint16_t layer = 0;
};
using MRT = std::array<TargetBufferInfo, MAX_SUPPORTED_RENDER_TARGET_COUNT>;

struct RenderTarget : public RefCounted {
    uint32_t width{};
    uint32_t height{};
    Ref<Texture> color[MAX_SUPPORTED_RENDER_TARGET_COUNT];
    Ref<Texture> depth;
    Ref<Texture> stencil;
    TargetBufferFlags targets = {};

    RenderTarget() noexcept = default;
    RenderTarget(uint32_t w, uint32_t h) : width(w), height(h) {}
};

struct PipelineState {
    Ref<Program> program;
};

}  // namespace Backend
}  // namespace Ethereal