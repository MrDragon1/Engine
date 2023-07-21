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

void VulkanPipelineCache::Init(VkDevice device) {
    mDevice = device;
    BindTopology(VK_PRIMITIVE_TOPOLOGY_LINE_LIST);
}

void VulkanPipelineCache::BindProgram(Ref<VulkanProgram> program) {
    mCurrentPipelineKey.shaders[0] = program->vertexShaderModule;
    mCurrentPipelineKey.shaders[1] = program->fragmentShaderModule;
}

void VulkanPipelineCache::BindRenderPass(VkRenderPass renderPass, uint16_t subpassIndex) {
    mCurrentPipelineKey.renderPass = renderPass;
    mCurrentPipelineKey.subpassIndex = subpassIndex;
}

void VulkanPipelineCache::BindRasterState(const RasterState& rasterState) {
    mCurrentPipelineKey.rasterState.cullMode = VK_CULL_MODE_BACK_BIT;
    mCurrentPipelineKey.rasterState.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    mCurrentPipelineKey.rasterState.depthBiasEnable = VK_FALSE;
    mCurrentPipelineKey.rasterState.blendEnable = VK_FALSE;
    mCurrentPipelineKey.rasterState.depthWriteEnable =
        rasterState.EnableDepthWrite ? VK_TRUE : VK_FALSE;
    mCurrentPipelineKey.rasterState.alphaToCoverageEnable = VK_FALSE;

    mCurrentPipelineKey.rasterState.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    mCurrentPipelineKey.rasterState.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    mCurrentPipelineKey.rasterState.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    mCurrentPipelineKey.rasterState.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    mCurrentPipelineKey.rasterState.rasterizationSamples = 1;
    mCurrentPipelineKey.rasterState.colorTargetCount = 2;
    mCurrentPipelineKey.rasterState.colorBlendOp = BlendEquation::ADD;
    mCurrentPipelineKey.rasterState.alphaBlendOp = BlendEquation::ADD;
    mCurrentPipelineKey.rasterState.depthCompareOp = rasterState.depthFunc;
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

    VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = GetOrCreatePipelineLayout();
    pipelineCreateInfo.renderPass = mCurrentPipelineKey.renderPass;
    pipelineCreateInfo.subpass = mCurrentPipelineKey.subpassIndex;
    pipelineCreateInfo.stageCount = hasFragmentShader ? SHADER_MODULE_COUNT : 1;
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
    vkDs.maxDepthBounds = 0.0f;

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

VkPipelineLayout VulkanPipelineCache::GetOrCreatePipelineLayout() {
    auto key = mCurrentPipelineKey.layout;
    auto it = mPipelineLayoutCache.find(key);
    if (it != mPipelineLayoutCache.end()) {
        return it->second.layout;
    }

    PipelineLayoutVal cacheEntry = {};

    VkDescriptorSetLayoutBinding binding = {};
    binding.descriptorCount = 1;                        // NOTE: We never use arrays-of-blocks.
    binding.stageFlags = VK_SHADER_STAGE_ALL_GRAPHICS;  // NOTE: This is potentially non-optimal.

    // First create the descriptor set layout for UBO's.
    VkDescriptorSetLayoutBinding ubindings[UBUFFER_BINDING_COUNT];
    binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
    return cacheEntry.layout;
}

}  // namespace Backend
}  // namespace Ethereal
