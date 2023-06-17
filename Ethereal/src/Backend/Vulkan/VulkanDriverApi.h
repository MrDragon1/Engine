#pragma once
#include "Backend/DriverApi.h"
#include "Backend/Vulkan/VulkanContext.h"

namespace Ethereal {
namespace Backend {
class VulkanDriverApi : public DriverApi {
   public:
    VulkanDriverApi();
    ~VulkanDriverApi();
    void Init();
    void Clean();

    Ref<VulkanContext> GetContext() { return mContext; }

    // DriverApi interface

    void BeginFrame() override;
    void EndFrame() override;

    virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) {}

    virtual Ref<Texture> CreateTexture(uint8_t levels, uint32_t width, uint32_t height,
                                       uint32_t depth, TextureFormat format, TextureUsage usage,
                                       TextureType type) {
        return nullptr;
    }
    virtual Ref<SamplerGroup> CreateSamplerGroup(uint32_t size) { return nullptr; }
    virtual Ref<BufferObject> CreateBufferObject(uint32_t byteCount,
                                                 BufferObjectBinding bindingType,
                                                 BufferUsage usage) {
        return nullptr;
    }
    virtual Ref<VertexBuffer> CreateVertexBuffer(AttributeArray attributeArray,
                                                 uint32_t vertexCount, uint8_t attributeCount,
                                                 uint8_t bufferCount) {
        return nullptr;
    }
    virtual Ref<IndexBuffer> CreateIndexBuffer(ElementType elementType, uint32_t indexCount,
                                               BufferUsage usage) {
        return nullptr;
    }
    virtual Ref<RenderPrimitive> CreateRenderPrimitive(Ref<VertexBuffer> vbh, Ref<IndexBuffer> ibh,
                                                       PrimitiveType pt, uint32_t offset,
                                                       uint32_t minIndex, uint32_t maxIndex,
                                                       uint32_t count) {
        return nullptr;
    }
    virtual Ref<Program> CreateProgram(std::string_view name, ShaderSource source) {
        return nullptr;
    }
    virtual Ref<Program> CreateProgram(std::string_view name, ShaderSourceString source) {
        return nullptr;
    }
    virtual Ref<RenderTarget> CreateRenderTarget(TargetBufferFlags targets, uint32_t width,
                                                 uint32_t height, MRT color, TargetBufferInfo depth,
                                                 TargetBufferInfo stencil) {
        return nullptr;
    }

    virtual void DestroyTexture(Ref<Texture> handle) {}
    virtual void DestroyRenderTarget(Ref<RenderTarget> handle) {}

    virtual void Draw(Ref<RenderPrimitive> rph, PipelineState pipeline) {}
    virtual void BeginRenderPass(RenderTargetHandle rth, const RenderPassParams& params) {}
    virtual void EndRenderPass() {}

    virtual void SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index,
                                       Ref<BufferObject> boh) {}
    virtual void UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd,
                                    uint32_t byteOffset) {}
    virtual void UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd,
                                   uint32_t byteOffset) {}
    virtual void SetTextureData(Ref<Texture> texture, uint32_t levels, uint32_t xoffset,
                                uint32_t yoffset, uint32_t zoffset, uint32_t width, uint32_t height,
                                uint32_t depth, const PixelBufferDescriptor& desc) {}
    virtual void UpdateSamplerGroup(SamplerGroupHandle sgh, SamplerGroupDescriptor& desc) {}
    virtual void BindSamplerGroup(uint8_t binding, SamplerGroupHandle sgh) {}
    virtual void BindUniformBuffer(uint8_t binding, BufferObjectHandle boh) {}

    virtual void GenerateMipmaps(TextureHandle th) {}
    virtual void SetRenderTargetAttachment(RenderTargetHandle rth, TargetBufferInfo const& info,
                                           TargetBufferFlags flag) {}
    virtual uint32_t GetTextueID(TextureHandle th) { return 0; }
    virtual void GetSubTexture(TextureHandle th, uint32_t layer, TextureHandle dst) {}
    virtual int ReadPixel(RenderTargetHandle rth, uint32_t attachmentIndex, uint32_t xoffset,
                          uint32_t yoffset) {
        return 0;
    }

    virtual void Clear() {}
    virtual uint32_t UseProgram(ProgramHandle program) { return 0; }
    virtual void BindUniform(ProgramHandle program, const string& name, ValueBasePtr value) {}

   private:
    Ref<VulkanContext> mContext;
};
}  // namespace Backend
}  // namespace Ethereal