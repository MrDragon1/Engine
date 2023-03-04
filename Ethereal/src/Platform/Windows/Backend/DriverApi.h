#pragma once

#include "BufferDescriptor.h"
#include "DriverBase.h"
#include "DriverEnums.h"
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
    virtual Ref<Texture> CreateTexture(uint32_t width, uint32_t height, uint32_t depth, TextureFormat format, TextureUsage usage,
                                       TextureType type) = 0;
    // virtual Ref<Sampler> CreateSampler() = 0;
    virtual Ref<BufferObject> CreateBufferObject(uint32_t byteCount, BufferObjectBinding bindingType, BufferUsage usage) = 0;
    virtual Ref<VertexBuffer> CreateVertexBuffer(AttributeArray attributeArray, uint32_t vertexCount, uint8_t attributeCount, uint8_t buffer) = 0;
    virtual Ref<IndexBuffer> CreateIndexBuffer(ElementType elementType, uint32_t indexCount, BufferUsage usage) = 0;
    virtual Ref<RenderPrimitive> CreateRenderPrimitive(Ref<VertexBuffer> vbh, Ref<IndexBuffer> ibh, PrimitiveType pt, uint32_t offset,
                                                       uint32_t minIndex, uint32_t maxIndex, uint32_t count) = 0;
    // virtual Ref<UniformBuffer> CreateUniformBuffer() = 0;
    virtual Ref<Program> CreateProgram(std::string_view name, ShaderSource source) = 0;

    virtual void Draw(Ref<RenderPrimitive> rp, Ref<Program> p) = 0;
    virtual void SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index, Ref<BufferObject> boh) = 0;
    virtual void UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd, uint32_t byteOffset) = 0;
    virtual void UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd, uint32_t byteOffset) = 0;

   private:
    /*
     * The function below is provided to DriverApi, is used as utilities for functions in public scope above
     */
   private:
};
}  // namespace Backend
}  // namespace Ethereal