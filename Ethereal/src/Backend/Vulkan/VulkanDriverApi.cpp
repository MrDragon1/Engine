#include "VulkanDriverApi.h"
#include "Core/Material/MaterialBase/Value.h"
#include "Backend/Vulkan/VulkanTexture.h"
#include "Backend/Vulkan/VulkanBuffer.h"
#include "backends/imgui_impl_vulkan.h"

#include "Utils/ShaderUtils.h"
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

    mPipelineCache = Ref<VulkanPipelineCache>::Create();
    mPipelineCache->Init(mContext->mDevice->GetDevice());
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
    ShaderSource binarySource;
    Utils::SpirvUtils::Init();
    for (auto [type, value] : source) {
        vector<uint32_t> binary;
        bool res = Utils::SpirvUtils::GLSL2SPV(value, type, binary);
        if (res) {
            binarySource[type].resize(binary.size() * sizeof(uint32_t));
            memcpy(binarySource[type].data(), binary.data(), binary.size() * sizeof(uint32_t));
        } else {
            ET_CORE_ERROR("Compile shader {} failed.", name);
        }
    }
    Utils::SpirvUtils::Shutdown();
    return CreateProgram(name, binarySource);
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

void VulkanDriverApi::Draw(Ref<RenderPrimitive> rph, PipelineState pipeline) {
    VulkanPipelineCache::VertexArray varray = {};
    VkBuffer buffers[MAX_VERTEX_ATTRIBUTE_COUNT] = {};
    VkDeviceSize offsets[MAX_VERTEX_ATTRIBUTE_COUNT] = {};

    // For each attribute, append to each of the above lists.
    const uint32_t bufferCount = rph->vertexBuffer->attributes.size();
    for (uint32_t attribIndex = 0; attribIndex < bufferCount; attribIndex++) {
        Attribute attrib = rph->vertexBuffer->attributes[attribIndex];

        const bool isInteger = attrib.flags & Attribute::FLAG_INTEGER_TARGET;
        const bool isNormalized = attrib.flags & Attribute::FLAG_NORMALIZED;

        VkFormat vkformat = VulkanUtils::GetVkFormat(attrib.type, isNormalized, isInteger);
        if (attrib.buffer == Attribute::BUFFER_UNUSED) {
            vkformat = isInteger ? VK_FORMAT_R8G8B8A8_UINT : VK_FORMAT_R8G8B8A8_SNORM;
            attrib = rph->vertexBuffer->attributes[0];
        }

        Ref<VulkanBuffer> buffer =
            rph->vertexBuffer.As<VulkanVertexBuffer>()->buffers[attrib.buffer];

        if (buffer == nullptr) {
            return;
        }

        buffers[attribIndex] = buffer->GetBuffer();
        offsets[attribIndex] = attrib.offset;
        varray.attributes[attribIndex] = {
            .location = attribIndex,  // matches the GLSL layout specifier
            .binding = attribIndex,   // matches the position within vkCmdBindVertexBuffers
            .format = vkformat,
        };
        varray.buffers[attribIndex] = {
            .binding = attribIndex,
            .stride = attrib.stride,
        };
    }

    mPipelineCache->BindProgram(pipeline.program);
    mPipelineCache->BindRasterState(pipeline.rasterState);
    mPipelineCache->BindRenderPass(mCurrentRenderPass.renderPass, 0);
    mPipelineCache->BindVertexArrays(varray);
    mPipelineCache->BindDescriptors(mCurrentRenderPass.commandBuffer);

    Viewport viewport = mCurrentRenderPass.params.viewport;
    mPipelineCache->BindScissor(
        mCurrentRenderPass.commandBuffer,
        VkRect2D{.offset = {0, 0},
                 .extent = {uint32_t(viewport.width), uint32_t(viewport.height)}});

    mPipelineCache->BindPipeline(mCurrentRenderPass.commandBuffer);

    // Bind the vertex buffers.
    vkCmdBindVertexBuffers(mCurrentRenderPass.commandBuffer, 0, bufferCount, buffers, offsets);

    // Bind the index buffer.
    if (rph->indexBuffer) {
        Ref<VulkanIndexBuffer> indexBuffer = rph->indexBuffer.As<VulkanIndexBuffer>();
        vkCmdBindIndexBuffer(mCurrentRenderPass.commandBuffer, indexBuffer->buffer->GetBuffer(), 0,
                             VK_INDEX_TYPE_UINT32);
    }
    const uint32_t indexCount = rph->count;
    const uint32_t firstIndex = rph->offset / rph->indexBuffer->elementSize;
    const int32_t vertexOffset = 0;
    const uint32_t firstInstId = 0;
    vkCmdDrawIndexed(mCurrentRenderPass.commandBuffer, indexCount, 1, firstIndex, vertexOffset,
                     firstInstId);
}

void VulkanDriverApi::BeginRenderPass(RenderTargetHandle rth, const RenderPassParams& params) {
    Ref<VulkanRenderTarget> rt = rth.As<VulkanRenderTarget>();
    const VkExtent2D extent = rt->GetExtent();

    VulkanAttachment depth = rt->GetDepth();

    mCurrentRenderPass.commandBuffer = mContext->mDevice->GetCommandBuffer(true);
    mCurrentRenderPass.isRecording = true;
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
                texture->TransitionLayout(mCurrentRenderPass.commandBuffer,
                                          VulkanLayout::DEPTH_SAMPLER, subresources);
                break;
            }
        }
    }

    VulkanLayout currentDepthLayout = depth.GetLayout();
    VulkanLayout renderPassDepthLayout = VulkanLayout::DEPTH_ATTACHMENT;
    VulkanLayout finalDepthLayout = renderPassDepthLayout;

    if (depth.texture) {
        if (currentDepthLayout != renderPassDepthLayout) {
            depth.texture->TransitionLayout(mCurrentRenderPass.commandBuffer, renderPassDepthLayout,
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
                    mCurrentRenderPass.commandBuffer, VulkanLayout::COLOR_ATTACHMENT,
                    info.GetSubresourceRange(VK_IMAGE_ASPECT_COLOR_BIT));
            }
        } else {
            rpkey.colorFormat[i] = VK_FORMAT_UNDEFINED;
        }
    }

    VkRenderPass renderPass = mFramebufferCache->GetRenderPass(rpkey);
    // mPipelineCache->BindRenderPass(renderPass);

    VulkanFramebufferCache::FramebufferKey fbkey{
        .renderPass = renderPass,
        .width = (uint16_t)extent.width,
        .height = (uint16_t)extent.height,
        .layers = 1,
        .samples = 1,
    };

    for (int i = 0; i < MAX_SUPPORTED_RENDER_TARGET_COUNT; i++) {
        if (!rt->GetColor(i).texture) {
            fbkey.color[i] = VK_NULL_HANDLE;
            fbkey.resolve[i] = VK_NULL_HANDLE;
        } else if (fbkey.samples == 1) {
            fbkey.color[i] = rt->GetColor(i).GetImageView(VK_IMAGE_ASPECT_COLOR_BIT);
            fbkey.resolve[i] = VK_NULL_HANDLE;
        }
    }
    if (depth.texture) {
        fbkey.depth = depth.GetImageView(VK_IMAGE_ASPECT_DEPTH_BIT);
    }
    VkFramebuffer vkfb = mFramebufferCache->GetFramebuffer(fbkey);

    VkRenderPassBeginInfo renderPassInfo{
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .renderPass = renderPass,
        .framebuffer = vkfb,

        // The renderArea field constrains the LoadOp, but scissoring does not.
        // Therefore, we do not set the scissor rect here, we only need it in draw().
        .renderArea = {.offset = {}, .extent = extent}};

    VkClearValue
        clearValues[MAX_SUPPORTED_RENDER_TARGET_COUNT + MAX_SUPPORTED_RENDER_TARGET_COUNT + 1] = {};
    if (params.flags.clearMask != TargetBufferFlags::NONE) {
        // NOTE: clearValues must be populated in the same order as the attachments array in
        // VulkanFboCache::getFramebuffer. Values must be provided regardless of whether Vulkan is
        // actually clearing that particular target.
        for (int i = 0; i < MAX_SUPPORTED_RENDER_TARGET_COUNT; i++) {
            if (fbkey.color[i]) {
                VkClearValue& clearValue = clearValues[renderPassInfo.clearValueCount++];
                clearValue.color.float32[0] = params.clearColor.x;
                clearValue.color.float32[1] = params.clearColor.y;
                clearValue.color.float32[2] = params.clearColor.z;
                clearValue.color.float32[3] = params.clearColor.w;
            }
        }

        if (fbkey.depth) {
            VkClearValue& clearValue = clearValues[renderPassInfo.clearValueCount++];
            clearValue.depthStencil = {(float)params.clearDepth, params.clearStencil};
        }
        renderPassInfo.pClearValues = &clearValues[0];
    }

    vkCmdBeginRenderPass(mCurrentRenderPass.commandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {.x = (float)params.viewport.left,
                           .y = (float)params.viewport.bottom,
                           .width = (float)params.viewport.width,
                           .height = (float)params.viewport.height,
                           .minDepth = params.depthRange.near_,
                           .maxDepth = params.depthRange.far_};

    vkCmdSetViewport(mCurrentRenderPass.commandBuffer, 0, 1, &viewport);

    mCurrentRenderPass.renderTarget = rt;
    mCurrentRenderPass.renderPass = renderPassInfo.renderPass;
    mCurrentRenderPass.params = params;
    mCurrentRenderPass.currentSubpass = 0;
}

void VulkanDriverApi::EndRenderPass() {
    vkCmdEndRenderPass(mCurrentRenderPass.commandBuffer);
    mCurrentRenderPass.isRecording = false;
    mContext->mDevice->FlushCommandBuffer(mCurrentRenderPass.commandBuffer);
    Ref<VulkanRenderTarget> rt = mCurrentRenderPass.renderTarget;

    mCurrentRenderPass.renderTarget = nullptr;
    mCurrentRenderPass.renderPass = VK_NULL_HANDLE;
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
    Ref<VulkanSamplerGroup> sg = sgh.As<VulkanSamplerGroup>();
    mSamplerGroupBindings[binding] = sg;
    // FIX: the param binding conflict with the binding info in sgh.
    // the binding info in param should be removed, since we always use the set=1 for texture
    // sampler
    for (const auto& desc : sg->descs) {
        VkSampler sampler = mSamplerCache->GetSampler(desc.params);

        Ref<VulkanTexture> texture = desc.texture.As<VulkanTexture>();
        if (!texture) continue;
        mPipelineCache->BindSampler(
            (uint32_t)desc.binding, sampler, texture->GetPrimaryImageView(),
            VulkanUtils::GetVkImageLayout(texture->GetPrimaryImageLayout()));
    }
}

void VulkanDriverApi::BindUniformBuffer(uint8_t binding, BufferObjectHandle boh, uint32_t offset,
                                        uint32_t size) {
    Ref<VulkanBufferObject> bo = boh.As<VulkanBufferObject>();
    const VkDeviceSize voffset = offset;
    const VkDeviceSize vsize = size ? size : bo->byteCount;
    mPipelineCache->BindUniformBuffer((uint32_t)binding, bo->buffer->GetBuffer(), voffset, vsize);
}

void VulkanDriverApi::SetRenderTargetAttachment(RenderTargetHandle rth,
                                                TargetBufferInfo const& info,
                                                TargetBufferFlags flag) {
    Ref<VulkanRenderTarget> rt = rth.As<VulkanRenderTarget>();
    // This will resize the render target
    auto valueForLevel = [](size_t level, size_t value) {
        return std::max(size_t(1), value >> level);
    };
    rt->width = valueForLevel(info.level, info.handle->width);
    rt->height = valueForLevel(info.level, info.handle->height);

    if (any(flag & TargetBufferFlags::COLOR_ALL)) {
        const size_t maxDrawBuffers = MAX_SUPPORTED_RENDER_TARGET_COUNT;
        for (size_t i = 0; i < maxDrawBuffers; i++) {
            if (any(flag & getTargetBufferFlagsAt(i))) {
                rt->color[i] = {info.handle, info.level, info.layer};
            }
        }
    }
    if ((flag & TargetBufferFlags::DEPTH_AND_STENCIL) == TargetBufferFlags::DEPTH_AND_STENCIL) {
        ET_CORE_ASSERT(false, "Not supported update stencil&depth framebuffer yet");
    }
    if (any(flag & TargetBufferFlags::DEPTH)) {
    }

    if (any(flag & TargetBufferFlags::STENCIL)) {
    }
}

TextureID VulkanDriverApi::GetTextueID(TextureHandle th) {
    Ref<VulkanTexture> vulkanTexture = th.As<VulkanTexture>();

    if (vulkanTexture->textureid == (TextureID)INVALID_UINT32) {
        auto imageview = vulkanTexture->GetPrimaryImageView();
        if (!imageview) {
            ET_CORE_WARN("Texture has no imageview!");
            return 0;
        }
        SamplerParams params = SamplerParams::Default();
        vulkanTexture->textureid = (TextureID)ImGui_ImplVulkan_AddTexture(
            mSamplerCache->GetSampler(params), imageview,
            VulkanUtils::GetVkImageLayout(vulkanTexture->GetPrimaryImageLayout()));
    }
    return vulkanTexture->textureid;
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
