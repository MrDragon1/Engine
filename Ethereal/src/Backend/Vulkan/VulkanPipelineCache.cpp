#include "VulkanPipelineCache.h"

namespace Ethereal {
namespace Backend {

size_t VulkanPipelineCache::PipelineLayoutKeyHashFn::operator()(
    const PipelineLayoutKey& key) const {
    std::hash<PipelineLayoutKey> hasher;
    return hasher(key);
}

bool VulkanPipelineCache::PipelineLayoutKeyEqual::operator()(const PipelineLayoutKey& k1,
                                                             const PipelineLayoutKey& k2) const {
    return k1 == k2;
}

bool VulkanPipelineCache::PipelineEqual::operator()(const PipelineKey& k1,
                                                    const PipelineKey& k2) const {
    return 0 == memcmp((const void*)&k1, (const void*)&k2, sizeof(k1));
}

bool VulkanPipelineCache::DescEqual::operator()(const DescriptorKey& k1,
                                                const DescriptorKey& k2) const {
    for (uint32_t i = 0; i < UBUFFER_BINDING_COUNT; i++) {
        if (k1.uniformBuffers[i] != k2.uniformBuffers[i]
            //||k1.uniformBufferOffsets[i] != k2.uniformBufferOffsets[i] ||
            // k1.uniformBufferSizes[i] != k2.uniformBufferSizes[i]
        ) {
            return false;
        }
    }
    for (uint32_t i = 0; i < SAMPLER_BINDING_COUNT; i++) {
        if (k1.samplers[i].sampler != k2.samplers[i].sampler ||
            k1.samplers[i].imageView != k2.samplers[i].imageView ||
            k1.samplers[i].imageLayout != k2.samplers[i].imageLayout) {
            return false;
        }
    }
    for (uint32_t i = 0; i < TARGET_BINDING_COUNT; i++) {
        if (k1.inputAttachments[i].imageView != k2.inputAttachments[i].imageView ||
            k1.inputAttachments[i].imageLayout != k2.inputAttachments[i].imageLayout) {
            return false;
        }
    }
    return true;
}

VulkanPipelineCache::VulkanPipelineCache() {
    mDummyBufferWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    mDummyBufferWriteInfo.pNext = nullptr;
    mDummyBufferWriteInfo.dstArrayElement = 0;
    mDummyBufferWriteInfo.descriptorCount = 1;
    mDummyBufferWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    mDummyBufferWriteInfo.pImageInfo = nullptr;
    mDummyBufferWriteInfo.pBufferInfo = &mDummyBufferInfo;
    mDummyBufferWriteInfo.pTexelBufferView = nullptr;

    mDummyTargetInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    mDummyTargetWriteInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    mDummyTargetWriteInfo.pNext = nullptr;
    mDummyTargetWriteInfo.dstArrayElement = 0;
    mDummyTargetWriteInfo.descriptorCount = 1;
    mDummyTargetWriteInfo.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    mDummyTargetWriteInfo.pImageInfo = &mDummyTargetInfo;
    mDummyTargetWriteInfo.pBufferInfo = nullptr;
    mDummyTargetWriteInfo.pTexelBufferView = nullptr;
}

void VulkanPipelineCache::Init(VkDevice device) {
    mDevice = device;
    CreateDescriptorPool(mDescriptorPoolSize);

    VkBufferCreateInfo bufferInfo{
        .sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
        .size = 16,
        .usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
    };
    VulkanAllocator::AllocateBuffer(bufferInfo, VMA_MEMORY_USAGE_GPU_ONLY, mDummyBuffer);

    mDummyBufferInfo.buffer = mDummyBuffer;
    mDummyBufferInfo.range = bufferInfo.size;

    BindTopology(VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST);
}

void VulkanPipelineCache::BindProgram(Ref<VulkanProgram> program) {
    mCurrentPipelineKey.shaders[0] = program->vertexShaderModule;
    mCurrentPipelineKey.shaders[1] = program->fragmentShaderModule;
    mCurrentPipelineKey.shaders[2] = program->geometryShaderModule;
}

void VulkanPipelineCache::BindRenderPass(VkRenderPass renderPass, uint16_t subpassIndex) {
    mCurrentPipelineKey.renderPass = renderPass;
    mCurrentPipelineKey.subpassIndex = subpassIndex;
}

void VulkanPipelineCache::BindRasterState(const RasterState& rasterState) {
    mCurrentPipelineKey.rasterState.cullMode = VK_CULL_MODE_NONE;
    mCurrentPipelineKey.rasterState.frontFace = VK_FRONT_FACE_CLOCKWISE;
    mCurrentPipelineKey.rasterState.depthBiasEnable = VK_FALSE;
    mCurrentPipelineKey.rasterState.blendEnable = VK_FALSE;
    mCurrentPipelineKey.rasterState.depthWriteEnable =
        rasterState.EnableDepthWrite ? VK_TRUE : VK_FALSE;
    mCurrentPipelineKey.rasterState.alphaToCoverageEnable = VK_FALSE;

    mCurrentPipelineKey.rasterState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    mCurrentPipelineKey.rasterState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    mCurrentPipelineKey.rasterState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    mCurrentPipelineKey.rasterState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    mCurrentPipelineKey.rasterState.rasterizationSamples = rasterState.rasterizationSamples;
    mCurrentPipelineKey.rasterState.colorTargetCount = rasterState.colorTargetCount;
    mCurrentPipelineKey.rasterState.colorBlendOp = BlendEquation::ADD;
    mCurrentPipelineKey.rasterState.alphaBlendOp = BlendEquation::ADD;
    mCurrentPipelineKey.rasterState.depthCompareOp = rasterState.depthFunc;

    mCurrentPipelineKey.rasterState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
        VK_COLOR_COMPONENT_A_BIT;
}

void VulkanPipelineCache::BindTopology(VkPrimitiveTopology topology) {
    mCurrentPipelineKey.topology = topology;
}

void VulkanPipelineCache::BindVertexArrays(const VertexArray& vertexArray) {
    for (uint32_t i = 0; i < VERTEX_ATTRIBUTE_COUNT; ++i) {
        mCurrentPipelineKey.vertexAttributes[i] = vertexArray.attributes[i];
        mCurrentPipelineKey.vertexBuffers[i] = vertexArray.buffers[i];
    }
}

void VulkanPipelineCache::BindScissor(VkCommandBuffer cmdbuffer, VkRect2D scissor) {
    mCurrentScissor = scissor;
    vkCmdSetScissor(cmdbuffer, 0, 1, &scissor);
}

void VulkanPipelineCache::BindPipeline(VkCommandBuffer cmdbuffer) {
    VkPipeline pipeline = GetOrCreatePipeline();

    vkCmdBindPipeline(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
}

void VulkanPipelineCache::BindDescriptors(VkCommandBuffer cmdbuffer) {
    auto key = mCurrentDescriptorKey;
    auto it = mDescriptorSetsCache.find(key);
    DescriptorVal* cache = nullptr;
    if (it != mDescriptorSetsCache.end()) {
        cache = &it->second;
    } else {
        cache = CreateDescriptorSets();
    }

    ET_CORE_ASSERT(cache, "Unable to load DescriptorSets for this pipeline!")

    std::array<uint32_t, UBUFFER_BINDING_COUNT> offsets = {0};
    for (int i = 0; i < UBUFFER_BINDING_COUNT; i++)
        offsets[i] = (uint32_t)key.uniformBufferOffsets[i];

    vkCmdBindDescriptorSets(cmdbuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,
                            GetOrCreatePipelineLayout()->layout, 0,
                            VulkanPipelineCache::DESCRIPTOR_TYPE_COUNT, cache->handles.data(),
                            UBUFFER_BINDING_COUNT, offsets.data());
}

VkPipeline VulkanPipelineCache::GetOrCreatePipeline() {
    auto key = mCurrentPipelineKey;
    auto it = mPipelineCache.find(key);
    if (it != mPipelineCache.end()) {
        return it->second.handle;
    }

    VkPipelineShaderStageCreateInfo shaderStages[SHADER_MODULE_COUNT] = {};
    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].module = key.shaders[0];
    shaderStages[0].pName = "main";

    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].module = key.shaders[1];
    shaderStages[1].pName = "main";

    shaderStages[2].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[2].stage = VK_SHADER_STAGE_GEOMETRY_BIT;
    shaderStages[2].module = key.shaders[2];
    shaderStages[2].pName = "main";

    VkPipelineColorBlendAttachmentState colorBlendAttachments[MAX_SUPPORTED_RENDER_TARGET_COUNT];
    VkPipelineColorBlendStateCreateInfo colorBlendState;
    colorBlendState = VkPipelineColorBlendStateCreateInfo{};
    colorBlendState.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendState.attachmentCount = 2;
    colorBlendState.pAttachments = colorBlendAttachments;

    uint32_t numVertexAttribs = 0;
    uint32_t numVertexBuffers = 0;
    VkVertexInputAttributeDescription vertexAttributes[VERTEX_ATTRIBUTE_COUNT];
    VkVertexInputBindingDescription vertexBuffers[VERTEX_ATTRIBUTE_COUNT];
    for (uint32_t i = 0; i < VERTEX_ATTRIBUTE_COUNT; i++) {
        if (mCurrentPipelineKey.vertexAttributes[i].format > 0) {
            vertexAttributes[numVertexAttribs] = mCurrentPipelineKey.vertexAttributes[i];
            numVertexAttribs++;
        }
        if (mCurrentPipelineKey.vertexBuffers[i].stride > 0) {
            vertexBuffers[numVertexBuffers] = mCurrentPipelineKey.vertexBuffers[i];
            numVertexBuffers++;
        }
    }

    VkPipelineVertexInputStateCreateInfo vertexInputState = {};
    vertexInputState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputState.vertexBindingDescriptionCount = numVertexBuffers;
    vertexInputState.pVertexBindingDescriptions = vertexBuffers;
    vertexInputState.vertexAttributeDescriptionCount = numVertexAttribs;
    vertexInputState.pVertexAttributeDescriptions = vertexAttributes;

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState = {};
    inputAssemblyState.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyState.topology = (VkPrimitiveTopology)mCurrentPipelineKey.topology;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkDynamicState dynamicStateEnables[] = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR,
    };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStateEnables;
    dynamicState.dynamicStateCount = 2;

    const bool hasFragmentShader = shaderStages[1].module != VK_NULL_HANDLE;
    const bool hasGeometryShader = shaderStages[2].module != VK_NULL_HANDLE;
    const int stageCount = hasGeometryShader ? (hasFragmentShader ? SHADER_MODULE_COUNT : 1) : 2;

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = GetOrCreatePipelineLayout()->layout;
    pipelineCreateInfo.renderPass = mCurrentPipelineKey.renderPass;
    pipelineCreateInfo.subpass = mCurrentPipelineKey.subpassIndex;
    pipelineCreateInfo.stageCount = stageCount;
    pipelineCreateInfo.pStages = shaderStages;
    pipelineCreateInfo.pVertexInputState = &vertexInputState;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;

    VkPipelineRasterizationStateCreateInfo vkRaster = {};
    vkRaster.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineCreateInfo.pRasterizationState = &vkRaster;

    VkPipelineMultisampleStateCreateInfo vkMs = {};
    vkMs.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineCreateInfo.pMultisampleState = &vkMs;

    VkPipelineDepthStencilStateCreateInfo vkDs = {};
    vkDs.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    vkDs.front = vkDs.back = {
        .failOp = VK_STENCIL_OP_KEEP,
        .passOp = VK_STENCIL_OP_KEEP,
        .depthFailOp = VK_STENCIL_OP_KEEP,
        .compareOp = VK_COMPARE_OP_ALWAYS,
        .compareMask = 0u,
        .writeMask = 0u,
        .reference = 0u,
    };
    pipelineCreateInfo.pDepthStencilState = &vkDs;

    const auto& raster = mCurrentPipelineKey.rasterState;

    vkRaster.polygonMode = VK_POLYGON_MODE_FILL;
    vkRaster.cullMode = raster.cullMode;
    vkRaster.frontFace = raster.frontFace;
    vkRaster.depthBiasEnable = raster.depthBiasEnable;
    vkRaster.depthBiasConstantFactor = raster.depthBiasConstantFactor;
    vkRaster.depthBiasClamp = 0.0f;
    vkRaster.depthBiasSlopeFactor = raster.depthBiasSlopeFactor;
    vkRaster.lineWidth = 1.0f;

    vkMs.rasterizationSamples = (VkSampleCountFlagBits)raster.rasterizationSamples;
    vkMs.sampleShadingEnable = VK_FALSE;
    vkMs.minSampleShading = 0.0f;
    vkMs.alphaToCoverageEnable = raster.alphaToCoverageEnable;
    vkMs.alphaToOneEnable = VK_FALSE;

    vkDs.depthTestEnable = VK_TRUE;
    vkDs.depthWriteEnable = raster.depthWriteEnable;
    vkDs.depthCompareOp = VulkanUtils::GetCompareOp(raster.depthCompareOp);
    vkDs.depthBoundsTestEnable = VK_FALSE;
    vkDs.stencilTestEnable = VK_FALSE;
    vkDs.minDepthBounds = 0.0f;
    vkDs.maxDepthBounds = 1.0f;

    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDynamicState = &dynamicState;

    // Filament assumes consistent blend state across all color attachments.
    colorBlendState.attachmentCount = mCurrentPipelineKey.rasterState.colorTargetCount;
    for (auto& target : colorBlendAttachments) {
        target.blendEnable = mCurrentPipelineKey.rasterState.blendEnable;
        target.srcColorBlendFactor = mCurrentPipelineKey.rasterState.srcColorBlendFactor;
        target.dstColorBlendFactor = mCurrentPipelineKey.rasterState.dstColorBlendFactor;
        target.colorBlendOp = (VkBlendOp)mCurrentPipelineKey.rasterState.colorBlendOp;
        target.srcAlphaBlendFactor = mCurrentPipelineKey.rasterState.srcAlphaBlendFactor;
        target.dstAlphaBlendFactor = mCurrentPipelineKey.rasterState.dstAlphaBlendFactor;
        target.alphaBlendOp = (VkBlendOp)mCurrentPipelineKey.rasterState.alphaBlendOp;
        target.colorWriteMask = mCurrentPipelineKey.rasterState.colorWriteMask;
    }

    // There are no color attachments if there is no bound fragment shader.  (e.g. shadow map gen)
    // TODO: This should be handled in a higher layer.
    if (!hasFragmentShader) {
        colorBlendState.attachmentCount = 0;
    }

    PipelineVal pipelineVal{};

    if (vkCreateGraphicsPipelines(mDevice, VK_NULL_HANDLE, 1, &pipelineCreateInfo, nullptr,
                                  &pipelineVal.handle) != VK_SUCCESS) {
    }

    mPipelineCache[mCurrentPipelineKey] = pipelineVal;
    return pipelineVal.handle;
}

VulkanPipelineCache::PipelineLayoutVal* VulkanPipelineCache::GetOrCreatePipelineLayout() {
    auto key = mCurrentPipelineKey.layout;
    auto it = mPipelineLayoutCache.find(key);
    if (it != mPipelineLayoutCache.end()) {
        return &it->second;
    }

    PipelineLayoutVal cacheEntry = {};

    VkDescriptorSetLayoutBinding binding = {};
    binding.descriptorCount = 1;                        // NOTE: We never use arrays-of-blocks.
    binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;  // NOTE: This is potentially non-optimal.

    // First create the descriptor set layout for UBO's.
    VkDescriptorSetLayoutBinding ubindings[UBUFFER_BINDING_COUNT];
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    for (uint32_t i = 0; i < UBUFFER_BINDING_COUNT; i++) {
        binding.binding = i;
        ubindings[i] = binding;
    }
    VkDescriptorSetLayoutCreateInfo dlinfo = {};
    dlinfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    dlinfo.bindingCount = UBUFFER_BINDING_COUNT;
    dlinfo.pBindings = ubindings;
    vkCreateDescriptorSetLayout(mDevice, &dlinfo, nullptr, &cacheEntry.descriptorSetLayouts[0]);

    // Next create the descriptor set layout for samplers.
    VkDescriptorSetLayoutBinding sbindings[SAMPLER_BINDING_COUNT];
    binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    for (uint32_t i = 0; i < SAMPLER_BINDING_COUNT; i++) {
        binding.stageFlags =
            VK_SHADER_STAGE_FRAGMENT_BIT;  // getShaderStageFlags(mCurrentPipelineKey.layout, i);
        binding.binding = i;
        sbindings[i] = binding;
    }
    dlinfo.bindingCount = SAMPLER_BINDING_COUNT;
    dlinfo.pBindings = sbindings;
    vkCreateDescriptorSetLayout(mDevice, &dlinfo, nullptr, &cacheEntry.descriptorSetLayouts[1]);

    // Next create the descriptor set layout for input attachments.
    VkDescriptorSetLayoutBinding tbindings[TARGET_BINDING_COUNT];
    binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
    for (uint32_t i = 0; i < TARGET_BINDING_COUNT; i++) {
        binding.binding = i;
        tbindings[i] = binding;
    }
    dlinfo.bindingCount = TARGET_BINDING_COUNT;
    dlinfo.pBindings = tbindings;
    vkCreateDescriptorSetLayout(mDevice, &dlinfo, nullptr, &cacheEntry.descriptorSetLayouts[2]);

    // Create VkPipelineLayout based on how to resources are bounded.
    VkPipelineLayoutCreateInfo pPipelineLayoutCreateInfo = {};
    pPipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pPipelineLayoutCreateInfo.setLayoutCount = cacheEntry.descriptorSetLayouts.size();
    pPipelineLayoutCreateInfo.pSetLayouts = cacheEntry.descriptorSetLayouts.data();
    VkResult result =
        vkCreatePipelineLayout(mDevice, &pPipelineLayoutCreateInfo, nullptr, &cacheEntry.layout);
    if (result != VK_SUCCESS) {
        return nullptr;
    }
    mPipelineLayoutCache.emplace(mCurrentPipelineKey.layout, cacheEntry);
    return &mPipelineLayoutCache[mCurrentPipelineKey.layout];
}

void VulkanPipelineCache::BindUniformBuffer(uint32_t bindingIndex, VkBuffer uniformBuffer,
                                            VkDeviceSize offset /*= 0*/,
                                            VkDeviceSize size /*= VK_WHOLE_SIZE*/) {
    ET_CORE_ASSERT(bindingIndex < UBUFFER_BINDING_COUNT,
                   "Uniform bindings overflow: index = %d, capacity = %d.", bindingIndex,
                   UBUFFER_BINDING_COUNT)
    auto& key = mCurrentDescriptorKey;
    key.uniformBuffers[bindingIndex] = uniformBuffer;

    key.uniformBufferOffsets[bindingIndex] = offset;
    key.uniformBufferSizes[bindingIndex] = size;
}

void VulkanPipelineCache::UnBindUniformBuffer(VkBuffer uniformBuffer) {
    auto& key = mCurrentDescriptorKey;
    for (uint32_t bindingIndex = 0u; bindingIndex < UBUFFER_BINDING_COUNT; ++bindingIndex) {
        if (key.uniformBuffers[bindingIndex] == uniformBuffer) {
            key.uniformBuffers[bindingIndex] = {};
            key.uniformBufferSizes[bindingIndex] = {};
            key.uniformBufferOffsets[bindingIndex] = {};
        }
    }
}

void VulkanPipelineCache::BindSampler(uint32_t bindingIndex, VkSampler sampler,
                                      VkImageView imageView, VkImageLayout imageLayout) {
    ET_CORE_ASSERT(bindingIndex < SAMPLER_BINDING_COUNT, "Sampler binding index out of range.");
    auto& key = mCurrentDescriptorKey;
    VkDescriptorImageInfo info{};
    info.sampler = sampler;
    info.imageView = imageView;
    info.imageLayout = imageLayout;
    key.samplers[bindingIndex] = info;
}

VulkanPipelineCache::DescriptorVal* VulkanPipelineCache::CreateDescriptorSets() {
    PipelineLayoutVal* pipelineLayoutCache = GetOrCreatePipelineLayout();

    DescriptorVal cache = {.pipelineLayout = mCurrentPipelineKey.layout};

    // Create descriptor sets in mCurrentDescriptorKey
    VkDescriptorSetAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = mDescriptorPool;
    allocInfo.descriptorSetCount = DESCRIPTOR_TYPE_COUNT;
    allocInfo.pSetLayouts = pipelineLayoutCache->descriptorSetLayouts.data();
    VkResult error = vkAllocateDescriptorSets(mDevice, &allocInfo, cache.handles.data());
    ET_CORE_ASSERT(error == VK_SUCCESS);

    // Rewrite every binding in the new descriptor sets.
    VkDescriptorBufferInfo descriptorBuffers[UBUFFER_BINDING_COUNT];
    VkDescriptorImageInfo descriptorSamplers[SAMPLER_BINDING_COUNT];
    VkDescriptorImageInfo descriptorInputAttachments[TARGET_BINDING_COUNT];
    VkWriteDescriptorSet
        descriptorWrites[UBUFFER_BINDING_COUNT + SAMPLER_BINDING_COUNT + TARGET_BINDING_COUNT];
    uint32_t nwrites = 0;
    VkWriteDescriptorSet* writes = descriptorWrites;
    nwrites = 0;
    for (uint32_t binding = 0; binding < UBUFFER_BINDING_COUNT; binding++) {
        VkWriteDescriptorSet& writeInfo = writes[nwrites++];
        if (mCurrentDescriptorKey.uniformBuffers[binding]) {
            VkDescriptorBufferInfo& bufferInfo = descriptorBuffers[binding];
            bufferInfo.buffer = mCurrentDescriptorKey.uniformBuffers[binding];
            // Since we are using dynamic uniform buffers, we don't need to specify the offset.
            // The offset will be specified in the draw call when vkCmdBindDescriptorSets() called.
            bufferInfo.offset = 0;  // mCurrentDescriptorKey.uniformBufferOffsets[binding];
            bufferInfo.range = mCurrentDescriptorKey.uniformBufferSizes[binding];

            writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeInfo.pNext = nullptr;
            writeInfo.dstArrayElement = 0;
            writeInfo.descriptorCount = 1;
            writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
            writeInfo.pImageInfo = nullptr;
            writeInfo.pBufferInfo = &bufferInfo;
            writeInfo.pTexelBufferView = nullptr;
        } else {
            writeInfo = mDummyBufferWriteInfo;
            ET_CORE_ASSERT(mDummyBufferWriteInfo.pBufferInfo->buffer);
        }
        ET_CORE_ASSERT(writeInfo.pBufferInfo->buffer);
        writeInfo.dstSet = cache.handles[0];
        writeInfo.dstBinding = binding;
    }
    for (uint32_t binding = 0; binding < SAMPLER_BINDING_COUNT; binding++) {
        if (mCurrentDescriptorKey.samplers[binding].sampler) {
            VkWriteDescriptorSet& writeInfo = writes[nwrites++];
            VkDescriptorImageInfo& imageInfo = descriptorSamplers[binding];
            imageInfo = mCurrentDescriptorKey.samplers[binding];
            writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            writeInfo.pNext = nullptr;
            writeInfo.dstArrayElement = 0;
            writeInfo.descriptorCount = 1;
            writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            writeInfo.pImageInfo = &imageInfo;
            writeInfo.pBufferInfo = nullptr;
            writeInfo.pTexelBufferView = nullptr;
            writeInfo.dstSet = cache.handles[1];
            writeInfo.dstBinding = binding;
        }
    }
    // for (uint32_t binding = 0; binding < TARGET_BINDING_COUNT; binding++) {
    //     VkWriteDescriptorSet& writeInfo = writes[nwrites++];
    //     if (mCurrentDescriptorKey.inputAttachments[binding].imageView) {
    //         VkDescriptorImageInfo& imageInfo = descriptorInputAttachments[binding];
    //         imageInfo = mCurrentDescriptorKey.inputAttachments[binding];
    //         writeInfo.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    //         writeInfo.pNext = nullptr;
    //         writeInfo.dstArrayElement = 0;
    //         writeInfo.descriptorCount = 1;
    //         writeInfo.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    //         writeInfo.pImageInfo = &imageInfo;
    //         writeInfo.pBufferInfo = nullptr;
    //         writeInfo.pTexelBufferView = nullptr;
    //     } else {
    //         writeInfo = mDummyTargetWriteInfo;
    //         //ET_CORE_ASSERT(mDummyTargetInfo.imageView);
    //     }
    //     writeInfo.dstSet = cache.handles[2];
    //     writeInfo.dstBinding = binding;
    // }
    vkUpdateDescriptorSets(mDevice, nwrites, writes, 0, nullptr);

    mDescriptorSetsCache[mCurrentDescriptorKey] = cache;
    return &mDescriptorSetsCache[mCurrentDescriptorKey];
}

void VulkanPipelineCache::CreateDescriptorPool(uint32_t size) {
    VkDescriptorPoolSize poolSizes[DESCRIPTOR_TYPE_COUNT] = {};
    VkDescriptorPoolCreateInfo poolInfo{.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
                                        .pNext = nullptr,
                                        .flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT,
                                        .maxSets = size * DESCRIPTOR_TYPE_COUNT,
                                        .poolSizeCount = DESCRIPTOR_TYPE_COUNT,
                                        .pPoolSizes = poolSizes};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    poolSizes[0].descriptorCount = poolInfo.maxSets * UBUFFER_BINDING_COUNT;
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = poolInfo.maxSets * SAMPLER_BINDING_COUNT;
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
    poolSizes[2].descriptorCount = poolInfo.maxSets * TARGET_BINDING_COUNT;

    const VkResult result = vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool);
    ET_CORE_ASSERT(result == VK_SUCCESS);
}

}  // namespace Backend
}  // namespace Ethereal
