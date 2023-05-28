#pragma once
#include <glad/glad.h>

#include <bitset>

#include "Backend/DriverApi.h"
#include "Backend/OpenGL/GLUtils.h"
namespace Ethereal {
namespace Backend {
/*!
 * @brief Map texture data from CPU to GPU, gl.id refers to the texture in OpenGL.
 * The actual data should upload with SetTextureData()
 */
struct GLTexture : public Texture {
    using Texture::Texture;
    struct {
        GLuint id;
        GLenum format;
        GLenum target;
        uint32_t baseLevel = INVALID_UINT32;
        uint32_t maxLevel = INVALID_UINT32;
    } gl;
};

struct GLSamplerGroup : public SamplerGroup {
    using SamplerGroup::SamplerGroup;

    struct Entry {
        Ref<Texture> texture;
        GLuint id;
        uint32_t binding = INVALID_UINT32;
        uint32_t level = INVALID_UINT32;
        uint32_t layer = INVALID_UINT32;
    };

    std::vector<Entry> entries;
    GLSamplerGroup(uint32_t size) : entries(size) {}
};

/*!
 * @brief Map data from CPU to GPU, gl.id refers to a buffer object in OpenGL.
 */
struct GLBufferObject : public BufferObject {
    using BufferObject::BufferObject;
    GLBufferObject(uint32_t size, BufferObjectBinding bindingType, BufferUsage usage)
        : BufferObject(size), usage(usage) {
        gl.binding = GLUtils::ResolveBufferObjectBinding(bindingType);
    }
    struct {
        GLuint id = 0;
        GLenum binding = 0;  // the binding type for this buffer
    } gl;
    BufferUsage usage = {};
};

/*!
 * @brief Define the data layout in buffer, the actual data can be accessed in GLBufferObject
 * It defines how attributes arrange and which buffer objects to use.
 */
struct GLVertexBuffer : public VertexBuffer {
    using VertexBuffer::VertexBuffer;
    struct {
        std::array<GLuint, MAX_VERTEX_ATTRIBUTE_COUNT>
            buffers{};  // map between vbo and attribute index
    } gl;
};

/*!
 * @brief Same As GLBufferObject, just extract the usage of ELEMENT_ARRAY_BUFFER as GLIndexBuffer
 */
struct GLIndexBuffer : public IndexBuffer {
    using IndexBuffer::IndexBuffer;
    struct {
        GLuint buffer{};
    } gl;
};

struct GLRenderPrimitive : public RenderPrimitive {
    using RenderPrimitive::RenderPrimitive;

    struct RenderPrimitive {
        static_assert(MAX_VERTEX_ATTRIBUTE_COUNT <= 16);

        GLuint vao = 0;
        uint8_t indicesSize = 0;

        GLenum getIndicesType() const noexcept {
            return indicesSize == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT;
        }
    } gl;
};

struct GLProgram : public Program {
    using Program::Program;
    struct {
        GLuint id;  // fbo
    } gl;
};

struct GLRenderTarget : public RenderTarget {
    using RenderTarget::RenderTarget;
    struct {
        GLuint id;
    } gl;
};

class OpenGLDriverApi : public DriverApi {
    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
    Ref<Texture> CreateTexture(uint8_t levels, uint32_t width, uint32_t height, uint32_t depth,
                               TextureFormat format, TextureUsage usage, TextureType type) override;
    Ref<SamplerGroup> CreateSamplerGroup(uint32_t size) override;
    Ref<BufferObject> CreateBufferObject(uint32_t byteCount, BufferObjectBinding bindingType,
                                         BufferUsage usage) override;
    Ref<VertexBuffer> CreateVertexBuffer(AttributeArray attributeArray, uint32_t vertexCount,
                                         uint8_t attributeCount, uint8_t bufferCount) override;
    Ref<IndexBuffer> CreateIndexBuffer(ElementType elementType, uint32_t indexCount,
                                       BufferUsage usage) override;
    Ref<RenderPrimitive> CreateRenderPrimitive(Ref<VertexBuffer> vbh, Ref<IndexBuffer> ibh,
                                               PrimitiveType pt, uint32_t offset, uint32_t minIndex,
                                               uint32_t maxIndex, uint32_t count) override;
    Ref<Program> CreateProgram(std::string_view name, ShaderSource source) override;
    Ref<Program> CreateProgram(std::string_view name, ShaderSourceString source) override;
    Ref<RenderTarget> CreateRenderTarget(TargetBufferFlags targets, uint32_t width, uint32_t height,
                                         MRT color, TargetBufferInfo depth,
                                         TargetBufferInfo stencil) override;

    void DestroyTexture(Ref<Texture> handle) override;
    void DestroyRenderTarget(Ref<RenderTarget> handle) override;

    void Draw(Ref<RenderPrimitive> rph, PipelineState pipeline) override;
    void BeginRenderPass(RenderTargetHandle rth, const RenderPassParams& params) override;
    void EndRenderPass() override;

    void SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index,
                               Ref<BufferObject> boh) override;
    void UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd,
                            uint32_t byteOffset) override;
    void UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd,
                           uint32_t byteOffset) override;
    void SetTextureData(Ref<Texture> texture, uint32_t levels, uint32_t xoffset, uint32_t yoffset,
                        uint32_t zoffset, uint32_t width, uint32_t height, uint32_t depth,
                        const PixelBufferDescriptor& desc) override;
    void UpdateSamplerGroup(SamplerGroupHandle sgh, SamplerGroupDescriptor& desc) override;
    void BindSamplerGroup(uint8_t binding, Ref<SamplerGroup> sgh) override;
    void BindUniformBuffer(uint8_t binding, BufferObjectHandle boh) override;

    void GenerateMipmaps(TextureHandle th) override;
    void SetRenderTargetAttachment(RenderTargetHandle rth, TargetBufferInfo const& info,
                                   TargetBufferFlags flag) override;

    uint32_t GetTextueID(TextureHandle th) override;
    void GetSubTexture(TextureHandle th, uint32_t layer, TextureHandle dst) override;
    int ReadPixel(RenderTargetHandle rth, uint32_t attachmentIndex, uint32_t xoffset,
                  uint32_t yoffset) override;
    void Clear() override;

   private:
    std::array<Ref<GLSamplerGroup>, 4> mSamplerGroupBindings;
    RenderTargetHandle mRenderPassTarget;
    RenderPassParams mRenderPassParams;

   private:
    void AllocateTexture(Ref<GLTexture> texture, uint32_t width, uint32_t height, uint32_t depth);
    void UpdateVertexArrayObject(Ref<GLRenderPrimitive> rp, Ref<GLVertexBuffer> vbh);
    void UpdateFrameBufferTexture(Ref<GLRenderTarget> rt, TargetBufferInfo const& info,
                                  GLenum attachment);
    void SetRasterState(RasterState state);
};

}  // namespace Backend
}  // namespace Ethereal