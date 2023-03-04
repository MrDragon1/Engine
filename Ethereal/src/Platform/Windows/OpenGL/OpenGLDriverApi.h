#pragma once
#include <glad/glad.h>

#include <bitset>

#include "Platform/Windows/Backend/DriverApi.h"
#include "Platform/Windows/OpenGL/GLUtils.h"
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
    } gl;
};

/*!
 * @brief Map data from CPU to GPU, gl.id refers to a buffer object in OpenGL.
 */
struct GLBufferObject : public BufferObject {
    using BufferObject::BufferObject;
    GLBufferObject(uint32_t size, BufferObjectBinding bindingType, BufferUsage usage) : BufferObject(size), usage(usage) {
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
        std::array<GLuint, MAX_VERTEX_ATTRIBUTE_COUNT> buffers{};  // map between vbo and attribute index
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

        GLuint vao = 0;           // 4
        uint8_t indicesSize = 0;  // 1

        GLenum getIndicesType() const noexcept { return indicesSize == 4 ? GL_UNSIGNED_INT : GL_UNSIGNED_SHORT; }
    } gl;
};

struct GLProgram : public Program {
    using Program::Program;
    struct {
        GLuint id;
    } gl;
};
class OpenGLDriverApi : public DriverApi {
    Ref<Texture> CreateTexture(uint32_t width, uint32_t height, uint32_t depth, TextureFormat format, TextureUsage usage, TextureType type) override;
    Ref<BufferObject> CreateBufferObject(uint32_t byteCount, BufferObjectBinding bindingType, BufferUsage usage) override;
    Ref<VertexBuffer> CreateVertexBuffer(AttributeArray attributeArray, uint32_t vertexCount, uint8_t attributeCount, uint8_t buffer) override;
    Ref<IndexBuffer> CreateIndexBuffer(ElementType elementType, uint32_t indexCount, BufferUsage usage) override;
    Ref<RenderPrimitive> CreateRenderPrimitive(Ref<VertexBuffer> vbh, Ref<IndexBuffer> ibh, PrimitiveType pt, uint32_t offset, uint32_t minIndex,
                                               uint32_t maxIndex, uint32_t count) override;
    Ref<Program> CreateProgram(std::string_view name, ShaderSource source) override;
    void Draw(Ref<RenderPrimitive> rp, Ref<Program> p) override;
    void SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index, Ref<BufferObject> boh) override;
    void UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd, uint32_t byteOffset) override;
    void UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd, uint32_t byteOffset) override;

   private:
    void AllocateTexture(Ref<GLTexture> texture, uint32_t width, uint32_t height, uint32_t depth);

    void SetTextureData(Ref<GLTexture> texture, uint32_t levels, uint32_t xoffset, uint32_t yoffset, uint32_t zoffset, uint32_t width,
                        uint32_t height, uint32_t depth, const PixelBufferDescriptor& desc);

    void UpdateVertexArrayObject(Ref<GLRenderPrimitive> rp, Ref<GLVertexBuffer> vbh);
};

}  // namespace Backend
}  // namespace Ethereal