#include "VulkanDriverApi.h"
#include "Core/Material/MaterialBase/Value.h"
#include "Backend/Vulkan/VulkanTexture.h"
#include "Backend/Vulkan/VulkanBuffer.h"
namespace Ethereal {
namespace Backend {
VulkanDriverApi::VulkanDriverApi() { Init(); }

VulkanDriverApi::~VulkanDriverApi() { Clean(); }

void VulkanDriverApi::Init() {
    mContext = Ref<VulkanContext>::Create();
    mContext->Init();

    mSamplerCache = Ref<VulkanSamplerCache>::Create();
    mSamplerCache->Init(mContext->mDevice->GetDevice());

    mFramebufferCache = Ref<VulkanFramebufferCache>::Create();
    mFramebufferCache->Init(mContext->mDevice->GetDevice());
}

void VulkanDriverApi::Clean() { mContext->Clean(); }

void VulkanDriverApi::BeginFrame() {
    mContext->mSwapchain->AcquireNextImage();

    mContext->mSwapchain->ResetCommandPool();
}

void VulkanDriverApi::EndFrame() { mContext->mSwapchain->Present(); }

Ref<Texture> VulkanDriverApi::CreateTexture(uint8_t levels, uint32_t width, uint32_t height,
                                            uint32_t depth, TextureFormat format,
                                            TextureUsage usage, TextureType type) {
    Ref<VulkanTexture> texture =
        Ref<VulkanTexture>::Create(mContext, width, height, depth, levels, format, usage, type);
    return texture;
}

Ref<SamplerGroup> VulkanDriverApi::CreateSamplerGroup(uint32_t size) {
    Ref<VulkanSamplerGroup> sg = Ref<VulkanSamplerGroup>::Create(size);
    return sg;
}

Ref<BufferObject> VulkanDriverApi::CreateBufferObject(uint32_t byteCount,
                                                      BufferObjectBinding bindingType,
                                                      BufferUsage usage) {
    Ref<VulkanBufferObject> handle =
        Ref<VulkanBufferObject>::Create(mContext, byteCount, bindingType, usage);
    return handle;
}

Ref<VertexBuffer> VulkanDriverApi::CreateVertexBuffer(AttributeArray attributeArray,
                                                      uint32_t vertexCount, uint8_t attributeCount,
                                                      uint8_t bufferCount) {
    Ref<VulkanVertexBuffer> handle = Ref<VulkanVertexBuffer>::Create(
        mContext, attributeArray, vertexCount, attributeCount, bufferCount);
    return handle;
}

Ref<IndexBuffer> VulkanDriverApi::CreateIndexBuffer(ElementType elementType, uint32_t indexCount,
                                                    BufferUsage usage) {
    uint8_t const elementSize = static_cast<uint8_t>(ResolveElementTypeSize(elementType));
    Ref<VulkanIndexBuffer> handle =
        Ref<VulkanIndexBuffer>::Create(mContext, elementSize, indexCount);
    return handle;
}

Ref<RenderPrimitive> VulkanDriverApi::CreateRenderPrimitive(Ref<VertexBuffer> vbh,
                                                            Ref<IndexBuffer> ibh, PrimitiveType pt,
                                                            uint32_t offset, uint32_t minIndex,
                                                            uint32_t maxIndex, uint32_t count) {
    Ref<VulkanVertexBuffer> vulkanVertexBuffer = vbh.As<VulkanVertexBuffer>();
    Ref<VulkanIndexBuffer> vulkanIndexBuffer = ibh.As<VulkanIndexBuffer>();
    Ref<VulkanRenderPrimitive> handle = Ref<VulkanRenderPrimitive>::Create();
    SetRenderPrimitiveBuffer(handle, vulkanVertexBuffer, vulkanIndexBuffer);
    SetRenderPrimitiveRange(handle, pt, offset, minIndex, maxIndex, count);

    return handle;
}

Ref<Program> VulkanDriverApi::CreateProgram(std::string_view name, ShaderSource source) {
    Ref<VulkanProgram> handle = Ref<VulkanProgram>::Create(mContext, name, source);

    return handle;
}

Ref<Program> VulkanDriverApi::CreateProgram(std::string_view name, ShaderSourceString source) {
    ET_CORE_WARN("Not support string shader source, please compile to shadersource first.");
    return nullptr;
}

Ref<RenderTarget> VulkanDriverApi::CreateRenderTarget(TargetBufferFlags targets, uint32_t width,
                                                      uint32_t height, MRT color,
                                                      TargetBufferInfo depth,
                                                      TargetBufferInfo stencil) {
    uint32_t tmin[2] = {std::numeric_limits<uint32_t>::max(), std::numeric_limits<uint32_t>::max()};
    uint32_t tmax[2] = {0, 0};
    size_t attachmentCount = 0;

    VulkanAttachment colorTargets[MAX_SUPPORTED_RENDER_TARGET_COUNT] = {};
    for (int i = 0; i < MAX_SUPPORTED_RENDER_TARGET_COUNT; i++) {
        if (color[i].handle) {
            colorTargets[i] = {
                .texture = color[i].handle,
                .level = color[i].level,
                .layer = color[i].layer,
            };
            VkExtent2D extent = colorTargets[i].GetExtent2D();
            tmin[0] = std::min(tmin[0], extent.width);
            tmin[1] = std::min(tmin[1], extent.height);
            tmax[0] = std::max(tmax[0], extent.width);
            tmax[1] = std::max(tmax[1], extent.height);
            attachmentCount++;
        }
    }

    VulkanAttachment depthStencil[2] = {};
    if (depth.handle) {
        depthStencil[0] = {
            .texture = depth.handle,
            .level = depth.level,
            .layer = depth.layer,
        };
        VkExtent2D extent = depthStencil[0].GetExtent2D();
        tmin[0] = std::min(tmin[0], extent.width);
        tmin[1] = std::min(tmin[1], extent.height);
        tmax[0] = std::max(tmax[0], extent.width);
        tmax[1] = std::max(tmax[1], extent.height);
        attachmentCount++;
    }

    if (stencil.handle) {
        depthStencil[1] = {
            .texture = stencil.handle,
            .level = stencil.level,
            .layer = stencil.layer,
        };
        VkExtent2D extent = depthStencil[1].GetExtent2D();
        tmin[0] = std::min(tmin[0], extent.width);
        tmin[1] = std::min(tmin[1], extent.height);
        tmax[0] = std::max(tmax[0], extent.width);
        tmax[1] = std::max(tmax[1], extent.height);
        attachmentCount++;
    }

    // All attachments must have the same dimensions, which must be greater than or equal to the
    // render target dimensions.
    ET_CORE_ASSERT(attachmentCount > 0);
    ET_CORE_ASSERT(tmin[0] == tmax[0] && tmin[1] == tmax[1]);
    ET_CORE_ASSERT(tmin[0] >= width && tmin[1] >= height);

    Ref<VulkanRenderTarget> rt =
        Ref<VulkanRenderTarget>::Create(mContext, width, height, colorTargets, depthStencil);

    return rt;
}

void VulkanDriverApi::BeginRenderPass(RenderTargetHandle rth, const RenderPassParams& params) {
    Ref<VulkanRenderTarget> rt = rth.As<VulkanRenderTarget>();
    const VkExtent2D extent = rt->GetExtent();

    VulkanAttachment depth = rt->GetDepth();

    VkCommandBuffer const cmdbuffer = mContext->mDevice->GetCommandBuffer(true);

    for (uint8_t samplerGroupIdx = 0; samplerGroupIdx < SAMPLER_BINDING_COUNT; samplerGroupIdx++) {
        Ref<VulkanSamplerGroup> vksb = mSamplerGroupBindings[samplerGroupIdx];
        if (!vksb) {
            continue;
        }
        std::vector<SamplerDescriptor> descs = vksb->descs;
        for (const auto& boundSampler : descs) {
            if (boundSampler.texture) {
                Ref<VulkanTexture> texture = boundSampler.texture.As<VulkanTexture>();
                if (!any(texture->usage & TextureUsage::DEPTH_ATTACHMENT)) {
                    continue;
                }

                VkImageSubresourceRange const subresources{
                    .aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT,
                    .baseMipLevel = 0,
                    .levelCount = texture->levels,
                    .baseArrayLayer = 0,
                    .layerCount = texture->depth,
                };
                texture->TransitionLayout(cmdbuffer, VulkanLayout::DEPTH_SAMPLER, subresources);
                break;
            }
        }
    }

    VulkanLayout currentDepthLayout = depth.GetLayout();
    VulkanLayout renderPassDepthLayout = VulkanLayout::DEPTH_ATTACHMENT;
    VulkanLayout finalDepthLayout = renderPassDepthLayout;

    if (depth.texture) {
        if (currentDepthLayout != renderPassDepthLayout) {
            depth.texture->TransitionLayout(cmdbuffer, renderPassDepthLayout,
                                            depth.GetSubresourceRange(VK_IMAGE_ASPECT_DEPTH_BIT));
            currentDepthLayout = renderPassDepthLayout;
        }
    }

    VulkanFramebufferCache::RenderPassKey rpkey = {
        .initialDepthLayout = currentDepthLayout,
        .renderPassDepthLayout = renderPassDepthLayout,
        .finalDepthLayout = finalDepthLayout,
        .depthFormat = depth.GetFormat(),
        .clearMask = params.flags.clearMask,
        .discardStart = params.flags.discardStart,
        .discardEnd = params.flags.discardEnd,
    };

    for (int i = 0; i < MAX_SUPPORTED_RENDER_TARGET_COUNT; i++) {
        VulkanAttachment info = rt->GetColor(i);
        if (info.texture) {
            rpkey.colorFormat[i] = info.GetFormat();
            if (info.texture->GetPrimaryImageLayout() != VulkanLayout::COLOR_ATTACHMENT) {
                info.texture.As<VulkanTexture>()->TransitionLayout(
                    cmdbuffer, VulkanLayout::COLOR_ATTACHMENT,
                    info.GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));
            }
        } else {
            rpkey.colorFormat[i] = VK_FORMAT_UNDEFINED;
        }
    }

    VkRenderPass renderPass = mFramebufferCache->GetRenderPass(rpkey);
}

void VulkanDriverApi::SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index,
                                            Ref<BufferObject> boh) {
    Ref<VulkanVertexBuffer> vulkanVertexBuffer = vbh.As<VulkanVertexBuffer>();
    Ref<VulkanBufferObject> vulkanBufferObject = boh.As<VulkanBufferObject>();
    vulkanVertexBuffer->buffers[index] = vulkanBufferObject->buffer;
}

void VulkanDriverApi::UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd,
                                         uint32_t byteOffset) {
    Ref<VulkanBufferObject> vulkanBuffer = handle.As<VulkanBufferObject>();
    vulkanBuffer->buffer->LoadData(bd, byteOffset);
}

void VulkanDriverApi::UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd,
                                        uint32_t byteOffset) {
    Ref<VulkanIndexBuffer> vulkanBuffer = handle.As<VulkanIndexBuffer>();
    vulkanBuffer->buffer->LoadData(bd, byteOffset);
}

void VulkanDriverApi::SetTextureData(Ref<Texture> texture, uint32_t levels, uint32_t xoffset,
                                     uint32_t yoffset, uint32_t zoffset, uint32_t width,
                                     uint32_t height, uint32_t depth,
                                     const PixelBufferDescriptor& desc) {
    Ref<VulkanTexture> vulkanTexture = texture.As<VulkanTexture>();
    vulkanTexture->UpdateData(desc, width, height, depth, xoffset, yoffset, zoffset, levels);
}

void VulkanDriverApi::UpdateSamplerGroup(SamplerGroupHandle sgh, SamplerGroupDescriptor& desc) {
    Ref<VulkanSamplerGroup> vulkanSamplerGroup = sgh.As<VulkanSamplerGroup>();
    for (size_t i = 0; i < desc.size(); ++i) {
        auto& entry = vulkanSamplerGroup->descs[i];
        if (desc[i].texture) {
            entry = desc[i];
        }
    }
}

void VulkanDriverApi::BindSamplerGroup(uint8_t binding, SamplerGroupHandle sgh) {
    mSamplerGroupBindings[binding] = sgh.As<VulkanSamplerGroup>();
}

TextureHandle VulkanDriverApi::GetColorAttachment(RenderTargetHandle rth,
                                                  uint32_t attachmentIndex) {
    Ref<VulkanRenderTarget> vulkanRenderTarget = rth.As<VulkanRenderTarget>();
    return vulkanRenderTarget->color[attachmentIndex].texture;
}

///////////////////////////  Private Functions  ///////////////////////////////////

void VulkanDriverApi::SetRenderPrimitiveBuffer(Ref<RenderPrimitive> rph, Ref<VertexBuffer> vbh,
                                               Ref<IndexBuffer> ibh) {
    Ref<VulkanRenderPrimitive> vulkanRenderPrimitive = rph.As<VulkanRenderPrimitive>();
    Ref<VulkanVertexBuffer> vulkanVertexBuffer = vbh.As<VulkanVertexBuffer>();
    Ref<VulkanIndexBuffer> vulkanIndexBuffer = ibh.As<VulkanIndexBuffer>();

    vulkanRenderPrimitive->SetBuffers(vulkanVertexBuffer, vulkanIndexBuffer);
}

void VulkanDriverApi::SetRenderPrimitiveRange(Ref<RenderPrimitive> rph, PrimitiveType pt,
                                              uint32_t offset, uint32_t minIndex, uint32_t maxIndex,
                                              uint32_t count) {
    Ref<VulkanRenderPrimitive> vulkanRenderPrimitive = rph.As<VulkanRenderPrimitive>();
    vulkanRenderPrimitive->SetPrimitiveType(pt);
    vulkanRenderPrimitive->offset = offset * vulkanRenderPrimitive->indexBuffer->elementSize;
    vulkanRenderPrimitive->count = count;
    vulkanRenderPrimitive->minIndex = minIndex;
    vulkanRenderPrimitive->maxIndex =
        maxIndex > minIndex ? maxIndex : vulkanRenderPrimitive->maxVertexCount - 1;
}

}  // namespace Backend
}  // namespace Ethereal
