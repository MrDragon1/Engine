#pragma once

#include "Core/Renderer/Buffer.h"

namespace Ethereal {
class OpenGLVertexBuffer : public VertexBuffer {
   public:
    OpenGLVertexBuffer(uint32_t size);
    OpenGLVertexBuffer(void* vertices, uint32_t size);
    virtual ~OpenGLVertexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual void SetData(const void* data, uint32_t size) override;

    virtual const BufferLayout& GetLayout() const override { return m_Layout; }
    virtual void SetLayout(const BufferLayout& layout) override { m_Layout = layout; }

   private:
    uint32_t m_RendererID;
    BufferLayout m_Layout;
};

class OpenGLIndexBuffer : public IndexBuffer {
   public:
    OpenGLIndexBuffer(void* indices, uint32_t size);
    virtual ~OpenGLIndexBuffer();

    virtual void Bind() const override;
    virtual void Unbind() const override;

    virtual uint32_t GetCount() const override { return m_Count; }

   private:
    uint32_t m_RendererID;
    uint32_t m_Count;
};

class OpenGLUniformBuffer : public UniformBuffer_ {
   public:
    OpenGLUniformBuffer(uint32_t size, uint32_t binding);
    ~OpenGLUniformBuffer();
    virtual void SetData(const void* data, uint32_t size, uint32_t offset = 0) override;

    virtual uint32_t GetBinding() const override { return m_Binding; }

   private:
    uint32_t m_RendererID;
    uint32_t m_Binding;
    uint32_t m_Size;
};

class OpenGLUniformBufferSet : public UniformBufferSet {
   public:
    OpenGLUniformBufferSet(uint32_t frames) : m_Frames(frames) {}

    virtual Ref<UniformBuffer_> Get(uint32_t frame, uint32_t binding) override {
        ET_CORE_ASSERT(m_UniformBuffers.find(frame) != m_UniformBuffers.end())
        ET_CORE_ASSERT(m_UniformBuffers.at(frame).find(binding) != m_UniformBuffers.at(frame).end())

        return m_UniformBuffers[frame][binding];
    }
    virtual void Set(Ref<UniformBuffer_> uniformBuffer, uint32_t frames = 0) override {
        m_UniformBuffers[frames][uniformBuffer->GetBinding()] = uniformBuffer;
    }

   private:
    uint32_t m_Frames;
    std::map<uint32_t, std::map<uint32_t, Ref<UniformBuffer_>>> m_UniformBuffers;
};

}  // namespace Ethereal