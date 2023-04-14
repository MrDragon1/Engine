#pragma once

#include "BufferDescriptor.h"
#include "DriverBase.h"
#include "DriverEnum.h"
#include "UniformBuffer.h"
namespace Ethereal {
namespace Backend {
/*
 * DriverApi aggregates the hardware operations that support by OpenGL & Vulkan. Only this class can
 * access the operation per platform. DriverApi abstract the platform and provide services for high
 * level layers.
 * */

class DriverApi : public RefCounted {
   public:
    ~DriverApi() = default;
    DriverApi() = default;
    static Ref<DriverApi> CreateApi(BackendType backend);
    /*
     * The function below is provided to Driver
     */
   public:
    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

    virtual Ref<Texture> CreateTexture(uint8_t levels, uint32_t width, uint32_t height, uint32_t depth, TextureFormat format, TextureUsage usage,
                                       TextureType type) = 0;
    virtual Ref<SamplerGroup> CreateSamplerGroup(uint32_t size) = 0;
    virtual Ref<BufferObject> CreateBufferObject(uint32_t byteCount, BufferObjectBinding bindingType, BufferUsage usage) = 0;
    virtual Ref<VertexBuffer> CreateVertexBuffer(AttributeArray attributeArray, uint32_t vertexCount, uint8_t attributeCount,
                                                 uint8_t bufferCount) = 0;
    virtual Ref<IndexBuffer> CreateIndexBuffer(ElementType elementType, uint32_t indexCount, BufferUsage usage) = 0;
    virtual Ref<RenderPrimitive> CreateRenderPrimitive(Ref<VertexBuffer> vbh, Ref<IndexBuffer> ibh, PrimitiveType pt, uint32_t offset,
                                                       uint32_t minIndex, uint32_t maxIndex, uint32_t count) = 0;
    virtual Ref<Program> CreateProgram(std::string_view name, ShaderSource source) = 0;
    virtual Ref<RenderTarget> CreateRenderTarget(TargetBufferFlags targets, uint32_t width, uint32_t height, MRT color, TargetBufferInfo depth,
                                                 TargetBufferInfo stencil) = 0;
    virtual void Draw(Ref<RenderPrimitive> rph, PipelineState pipeline) = 0;
    virtual void BeginRenderPass(RenderTargetHandle rth, const RenderPassParams& params) = 0;
    virtual void EndRenderPass() = 0;

    virtual void SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index, Ref<BufferObject> boh) = 0;
    virtual void UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd, uint32_t byteOffset) = 0;
    virtual void UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd, uint32_t byteOffset) = 0;
    virtual void SetTextureData(Ref<Texture> texture, uint32_t levels, uint32_t xoffset, uint32_t yoffset, uint32_t zoffset, uint32_t width,
                                uint32_t height, uint32_t depth, const PixelBufferDescriptor& desc) = 0;
    virtual void UpdateSamplerGroup(SamplerGroupHandle sgh, SamplerGroupDescriptor& desc) = 0;
    virtual void BindSamplerGroup(uint8_t binding, SamplerGroupHandle sgh) = 0;
    virtual void BindUniformBuffer(uint8_t binding, BufferObjectHandle boh) = 0;

    virtual void GenerateMipmaps(TextureHandle th) = 0;
    virtual void SetRenderTargetAttachment(RenderTargetHandle rth, TargetBufferInfo const& info, TargetBufferFlags flag) = 0;
    virtual uint32_t GetTextueID(TextureHandle th) = 0;
    virtual void GetSubTexture(TextureHandle th, uint32_t layer, TextureHandle dst) = 0;

    virtual void Clear() = 0;

   private:
    /*
     * The function below is provided to DriverApi, is used as utilities for functions in public scope above
     */
   private:
};
}  // namespace Backend
}  // namespace Ethereal
