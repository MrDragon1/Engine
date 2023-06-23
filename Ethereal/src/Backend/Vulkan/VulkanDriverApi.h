#pragma once
#include "Backend/DriverApi.h"
#include "Backend/Vulkan/VulkanContext.h"
#include "Backend/Vulkan/VulkanBase.h"

#include "Backend/Vulkan/VulkanSamplerCache.h"
#include "Backend/Vulkan/VulkanFramebufferCache.h"

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
                                       TextureType type) override;
    virtual Ref<SamplerGroup> CreateSamplerGroup(uint32_t size) override;
    virtual Ref<BufferObject> CreateBufferObject(uint32_t byteCount,
                                                 BufferObjectBinding bindingType,
                                                 BufferUsage usage) override;
    virtual Ref<VertexBuffer> CreateVertexBuffer(AttributeArray attributeArray,
                                                 uint32_t vertexCount, uint8_t attributeCount,
                                                 uint8_t bufferCount) override;
    virtual Ref<IndexBuffer> CreateIndexBuffer(ElementType elementType, uint32_t indexCount,
                                               BufferUsage usage) override;
    virtual Ref<RenderPrimitive> CreateRenderPrimitive(Ref<VertexBuffer> vbh, Ref<IndexBuffer> ibh,
                                                       PrimitiveType pt, uint32_t offset,
                                                       uint32_t minIndex, uint32_t maxIndex,
                                                       uint32_t count) override;
    virtual Ref<Program> CreateProgram(std::string_view name, ShaderSource source) override;
    virtual Ref<Program> CreateProgram(std::string_view name, ShaderSourceString source) override;
    virtual Ref<RenderTarget> CreateRenderTarget(TargetBufferFlags targets, uint32_t width,
                                                 uint32_t height, MRT color, TargetBufferInfo depth,
                                                 TargetBufferInfo stencil) override;

    virtual void DestroyTexture(Ref<Texture> handle) {}
    virtual void DestroyRenderTarget(Ref<RenderTarget> handle) {}

    virtual void Draw(Ref<RenderPrimitive> rph, PipelineState pipeline) {}
    virtual void BeginRenderPass(RenderTargetHandle rth, const RenderPassParams& params) override;
    virtual void EndRenderPass() {}

    virtual void SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index,
                                       Ref<BufferObject> boh) override;
    virtual void UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd,
                                    uint32_t byteOffset) override;
    virtual void UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd,
                                   uint32_t byteOffset) override;
    virtual void SetTextureData(Ref<Texture> texture, uint32_t levels, uint32_t xoffset,
                                uint32_t yoffset, uint32_t zoffset, uint32_t width, uint32_t height,
                                uint32_t depth, const PixelBufferDescriptor& desc) override;
    virtual void UpdateSamplerGroup(SamplerGroupHandle sgh, SamplerGroupDescriptor& desc) override;
    virtual void BindSamplerGroup(uint8_t binding, SamplerGroupHandle sgh) override;
    virtual void BindUniformBuffer(uint8_t binding, BufferObjectHandle boh){};

    virtual void GenerateMipmaps(TextureHandle th) {}
    virtual void SetRenderTargetAttachment(RenderTargetHandle rth, TargetBufferInfo const& info,
                                           TargetBufferFlags flag) {}
    virtual uint32_t GetTextueID(TextureHandle th) { return 0; }
    virtual void GetSubTexture(TextureHandle th, uint32_t layer, TextureHandle dst) {}
    virtual int ReadPixel(RenderTargetHandle rth, uint32_t attachmentIndex, uint32_t xoffset,
                          uint32_t yoffset) {
        return 0;
    }
    virtual TextureHandle GetColorAttachment(RenderTargetHandle rth,
                                             uint32_t attachmentIndex) override;
    virtual void Clear() {}
    virtual uint32_t UseProgram(ProgramHandle program) { return 0; }
    virtual void BindUniform(ProgramHandle program, const string& name, ValueBasePtr value) {}

   private:
    void SetRenderPrimitiveBuffer(Ref<RenderPrimitive> rph, Ref<VertexBuffer> vbh,
                                  Ref<IndexBuffer> ibh);
    void SetRenderPrimitiveRange(Ref<RenderPrimitive> rph, PrimitiveType pt, uint32_t offset,
                                 uint32_t minIndex, uint32_t maxIndex, uint32_t count);

   private:
    Ref<VulkanContext> mContext;

    std::array<Ref<VulkanSamplerGroup>, SAMPLER_BINDING_COUNT> mSamplerGroupBindings;

    Ref<VulkanSamplerCache> mSamplerCache;
    Ref<VulkanFramebufferCache> mFramebufferCache;
};
}  // namespace Backend
}  // namespace Ethereal