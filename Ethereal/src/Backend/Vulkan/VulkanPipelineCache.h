#pragma once
#include "Backend/Vulkan/VulkanUtils.h"
#include "Backend/Vulkan/VulkanBase.h"
#include "Base/Math/Hash.h"

namespace Ethereal {
namespace Backend {
class VulkanPipelineCache : public RefCounted {
   public:
    static constexpr uint32_t SHADER_MODULE_COUNT = 2;
    static constexpr uint32_t VERTEX_ATTRIBUTE_COUNT = MAX_VERTEX_ATTRIBUTE_COUNT;
    struct VertexArray {
        VkVertexInputAttributeDescription attributes[VERTEX_ATTRIBUTE_COUNT];
        VkVertexInputBindingDescription buffers[VERTEX_ATTRIBUTE_COUNT];
    };
    struct VulkanRasterState {
        VkCullModeFlags cullMode : 2;
        VkFrontFace frontFace : 2;
        VkBool32 depthBiasEnable : 1;
        VkBool32 blendEnable : 1;
        VkBool32 depthWriteEnable : 1;
        VkBool32 alphaToCoverageEnable : 1;
        VkBlendFactor srcColorBlendFactor : 5;  // offset = 1 byte
        VkBlendFactor dstColorBlendFactor : 5;
        VkBlendFactor srcAlphaBlendFactor : 5;
        VkBlendFactor dstAlphaBlendFactor : 5;
        VkColorComponentFlags colorWriteMask : 4;
        uint8_t rasterizationSamples;    // offset = 4 bytes
        uint8_t colorTargetCount;        // offset = 5 bytes
        BlendEquation colorBlendOp : 4;  // offset = 6 bytes
        BlendEquation alphaBlendOp : 4;
        SamplerCompareFunc depthCompareOp;  // offset = 7 bytes
        float depthBiasConstantFactor;      // offset = 8 bytes
        float depthBiasSlopeFactor;         // offset = 12 bytes
    };
    using PipelineLayoutKey = std::bitset<128>;

    struct PipelineLayoutKeyHashFn {
        size_t operator()(const PipelineLayoutKey& key) const;
    };
    static constexpr uint32_t DESCRIPTOR_TYPE_COUNT = 3;
    static constexpr uint32_t UBUFFER_BINDING_COUNT = 15u;
    static constexpr uint32_t TARGET_BINDING_COUNT = 32u;

    struct PipelineLayoutVal {
        VkPipelineLayout layout;
        std::array<VkDescriptorSetLayout, DESCRIPTOR_TYPE_COUNT> descriptorSetLayouts;
    };
    struct PipelineLayoutKeyEqual {
        bool operator()(const PipelineLayoutKey& k1, const PipelineLayoutKey& k2) const;
    };

    struct PipelineKey {
        VkShaderModule shaders[SHADER_MODULE_COUNT];                                 //  16  : 0
        VkRenderPass renderPass;                                                     //  8   : 16
        uint16_t topology;                                                           //  2   : 24
        uint16_t subpassIndex;                                                       //  2   : 26
        VkVertexInputAttributeDescription vertexAttributes[VERTEX_ATTRIBUTE_COUNT];  //  128 : 28
        VkVertexInputBindingDescription vertexBuffers[VERTEX_ATTRIBUTE_COUNT];       //  128 : 156
        VulkanRasterState rasterState;                                               //  16  : 284
        uint32_t padding;                                                            //  4   : 300
        PipelineLayoutKey layout;                                                    // 16   : 304
    };

    using PipelineHashFn = Math::Hash::MurmurHashFn<PipelineKey>;
    struct PipelineVal {
        VkPipeline handle;
    };
    struct PipelineEqual {
        bool operator()(const PipelineKey& k1, const PipelineKey& k2) const;
    };

    struct DescriptorKey {
        VkBuffer uniformBuffers[UBUFFER_BINDING_COUNT];
        VkDescriptorImageInfo samplers[SAMPLER_BINDING_COUNT];
        VkDescriptorImageInfo inputAttachments[TARGET_BINDING_COUNT];
        uint64_t uniformBufferOffsets[UBUFFER_BINDING_COUNT];
        uint64_t uniformBufferSizes[UBUFFER_BINDING_COUNT];
    };
    using DescHashFn = Math::Hash::MurmurHashFn<DescriptorKey>;
    struct DescriptorVal {
        std::array<VkDescriptorSet, DESCRIPTOR_TYPE_COUNT> handles;
        PipelineLayoutKey pipelineLayout;
    };
    struct DescEqual {
        bool operator()(const DescriptorKey& k1, const DescriptorKey& k2) const;
    };

    VulkanPipelineCache();
    void Init(VkDevice device);

    void BindProgram(Ref<VulkanProgram>);
    void BindRenderPass(VkRenderPass renderPass, uint16_t subpassIndex);
    void BindRasterState(const RasterState& rasterState);
    void BindTopology(VkPrimitiveTopology topology);
    void BindVertexArrays(const VertexArray& vertexArray);
    void BindScissor(VkCommandBuffer cmdbuffer, VkRect2D scissor);
    void BindPipeline(VkCommandBuffer);
    void BindDescriptors(VkCommandBuffer);

    VkPipeline GetOrCreatePipeline();
    PipelineLayoutVal* GetOrCreatePipelineLayout();

    void BindUniformBuffer(uint32_t bindingIndex, VkBuffer uniformBuffer, VkDeviceSize offset = 0,
                           VkDeviceSize size = VK_WHOLE_SIZE);
    void UnBindUniformBuffer(VkBuffer uniformBuffer);

    void BindSampler(uint32_t bindingIndex, VkSampler sampler, VkImageView imageView,
                     VkImageLayout imageLayout);
    void UnBindSampler(VkSampler sampler);

   private:
    DescriptorVal* CreateDescriptorSets();
    void CreateDescriptorPool(uint32_t size);

   private:
    VkDevice mDevice;
    DescriptorKey mCurrentDescriptorKey{};
    PipelineKey mCurrentPipelineKey{};
    VkRect2D mCurrentScissor = {};
    VkDescriptorPool mDescriptorPool;
    uint32_t mDescriptorPoolSize = 512;

    unordered_map<PipelineKey, PipelineVal, PipelineHashFn, PipelineEqual> mPipelineCache;
    unordered_map<PipelineLayoutKey, PipelineLayoutVal, PipelineLayoutKeyHashFn,
                  PipelineLayoutKeyEqual>
        mPipelineLayoutCache;

    unordered_map<DescriptorKey, DescriptorVal, DescHashFn, DescEqual> mDescriptorSetsCache;

    VkDescriptorBufferInfo mDummyBufferInfo = {};
    VkWriteDescriptorSet mDummyBufferWriteInfo = {};
    VkDescriptorImageInfo mDummyTargetInfo = {};
    VkWriteDescriptorSet mDummyTargetWriteInfo = {};

    VkBuffer mDummyBuffer;
};
}  // namespace Backend
}  // namespace Ethereal
