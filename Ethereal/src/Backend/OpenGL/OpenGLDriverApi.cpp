#include "OpenGLDriverApi.h"

#include <gl/glcorearb.h>

#include "GLUtils.h"
namespace Ethereal {
namespace Backend {

void OpenGLDriverApi::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) { glViewport(x, y, width, height); }

Ref<Texture> OpenGLDriverApi::CreateTexture(uint8_t levels, uint32_t width, uint32_t height, uint32_t depth, TextureFormat format, TextureUsage usage,
                                            TextureType type) {
    Ref<GLTexture> t = Ref<GLTexture>::Create(width, height, depth, levels, format, usage, type);
    if (any(usage & TextureUsage::SAMPLEABLE)) {
        t->gl.format = GLUtils::ResolveTextureFormat(format);
        t->gl.target = GLUtils::ResolveTextureType(type);

        glGenTextures(1, &t->gl.id);
        glBindTexture(t->gl.target, t->gl.id);

        AllocateTexture(t, width, height, depth);
    } else if (any(usage & (TextureUsage::COLOR_ATTACHMENT | TextureUsage::DEPTH_ATTACHMENT))) {
        t->gl.format = GLUtils::ResolveTextureFormat(format);
        t->gl.target = GL_RENDERBUFFER;
        glGenRenderbuffers(1, &t->gl.id);

        glBindRenderbuffer(GL_RENDERBUFFER, t->gl.id);
        glRenderbufferStorage(GL_RENDERBUFFER, t->gl.format, width, height);
        glBindRenderbuffer(GL_RENDERBUFFER, 0);
    }
    return t;
}

Ref<SamplerGroup> OpenGLDriverApi::CreateSamplerGroup(uint32_t size) {
    Ref<GLSamplerGroup> sg = Ref<GLSamplerGroup>::Create(size);
    return sg;
}

Ref<BufferObject> OpenGLDriverApi::CreateBufferObject(uint32_t byteCount, BufferObjectBinding bindingType, BufferUsage usage) {
    Ref<GLBufferObject> handle = Ref<GLBufferObject>::Create(byteCount, bindingType, usage);
    glGenBuffers(1, &handle->gl.id);

    // bind buffer to GL_ARRAY_BUFFER will change the current binding vertex array,
    // so must bind a dummy vertex array before binding BufferObject.
    // Use glBindVertexArray(0) to break the existing vertex array object binding.
    if (handle->gl.binding == GL_ARRAY_BUFFER) {
        glBindVertexArray(0);
    }

    glBindBuffer(handle->gl.binding, handle->gl.id);
    glBufferData(handle->gl.binding, byteCount, nullptr, GLUtils::ResolveBufferUsage(handle->usage));

    return handle;
}

Ref<VertexBuffer> OpenGLDriverApi::CreateVertexBuffer(AttributeArray attributeArray, uint32_t vertexCount, uint8_t attributeCount,
                                                      uint8_t bufferCount) {
    Ref<GLVertexBuffer> vb = Ref<GLVertexBuffer>::Create(attributeArray, vertexCount, attributeCount, bufferCount);
    return vb;
}

Ref<IndexBuffer> OpenGLDriverApi::CreateIndexBuffer(ElementType elementType, uint32_t indexCount, BufferUsage usage) {
    uint8_t const elementSize = static_cast<uint8_t>(ResolveElementTypeSize(elementType));
    Ref<GLIndexBuffer> ib = Ref<GLIndexBuffer>::Create(elementSize, indexCount);
    glGenBuffers(1, &ib->gl.buffer);
    GLsizeiptr const size = elementSize * indexCount;
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->gl.buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, nullptr, GLUtils::ResolveBufferUsage(usage));

    return ib;
}

Ref<RenderPrimitive> OpenGLDriverApi::CreateRenderPrimitive(Ref<VertexBuffer> vbh, Ref<IndexBuffer> ibh, PrimitiveType pt, uint32_t offset,
                                                            uint32_t minIndex, uint32_t maxIndex, uint32_t count) {
    Ref<GLVertexBuffer> eb = vbh.As<GLVertexBuffer>();
    Ref<GLIndexBuffer> ib = ibh.As<GLIndexBuffer>();

    Ref<GLRenderPrimitive> rp = Ref<GLRenderPrimitive>::Create();
    rp->gl.indicesSize = (ib->elementSize == 4u) ? 4u : 2u;
    rp->type = pt;
    rp->offset = offset * rp->gl.indicesSize;
    rp->count = count;
    rp->minIndex = minIndex;
    rp->maxIndex = maxIndex > minIndex ? maxIndex : rp->maxVertexCount - 1;  // sanitize max index
    rp->vertexBuffer = vbh;
    rp->indexBuffer = ibh;
    glGenVertexArrays(1, &rp->gl.vao);
    glBindVertexArray(rp->gl.vao);

    // update the VBO bindings in the VAO
    UpdateVertexArrayObject(rp, eb);

    return rp;
}

Ref<Program> OpenGLDriverApi::CreateProgram(std::string_view name, ShaderSource source) {
    Ref<GLProgram> p = Ref<GLProgram>::Create(name);
    p->gl.id = glCreateProgram();

    std::vector<GLenum> glShaderIDs;
    for (auto& kv : source) {
        GLenum glShaderType = GLUtils::ResolveShaderType(kv.first);

        ShaderBlob& blob = kv.second;

        GLuint shader = glCreateShader(glShaderType);

        glShaderBinary(1, &shader, GL_SHADER_BINARY_FORMAT_SPIR_V_ARB, blob.data(), blob.size());
        glSpecializeShader(shader, "main", 0, nullptr, nullptr);

        GLint isCompiled = 0;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
        if (isCompiled == GL_FALSE) {
            glDeleteShader(shader);
            ET_CORE_ASSERT(false, "Shader compilation failure!")
            break;
        }

        glAttachShader(p->gl.id, shader);
        glShaderIDs.push_back(shader);
    }

    // Link our program
    glLinkProgram(p->gl.id);

    // Note the different functions here: glGetProgram* instead of glGetShader*.
    GLint isLinked = 0;
    glGetProgramiv(p->gl.id, GL_LINK_STATUS, (int*)&isLinked);
    if (isLinked == GL_FALSE) {
        GLint maxLength = 0;
        glGetProgramiv(p->gl.id, GL_INFO_LOG_LENGTH, &maxLength);

        // The maxLength includes the NULL character
        std::vector<GLchar> infoLog(maxLength);
        glGetProgramInfoLog(p->gl.id, maxLength, &maxLength, &infoLog[0]);

        // We don't need the program anymore.
        glDeleteProgram(p->gl.id);

        for (auto id : glShaderIDs) glDeleteShader(id);

        ET_CORE_ERROR("{0}", infoLog.data());
        ET_CORE_ASSERT(false, "OpenGLShader link failure!")
    }

    for (auto id : glShaderIDs) glDetachShader(p->gl.id, id);

    return p;
}

Ref<RenderTarget> OpenGLDriverApi::CreateRenderTarget(TargetBufferFlags targets, uint32_t width, uint32_t height, MRT color, TargetBufferInfo depth,
                                                      TargetBufferInfo stencil) {
    Ref<GLRenderTarget> rt = Ref<GLRenderTarget>::Create(width, height);
    glGenFramebuffers(1, &rt->gl.id);
    glBindFramebuffer(GL_FRAMEBUFFER, rt->gl.id);
    rt->targets = targets;
    rt->width = width;
    rt->height = height;
    if (any(targets & TargetBufferFlags::COLOR_ALL)) {
        GLenum bufs[MAX_SUPPORTED_RENDER_TARGET_COUNT] = {GL_NONE};
        const size_t maxDrawBuffers = MAX_SUPPORTED_RENDER_TARGET_COUNT;
        for (size_t i = 0; i < maxDrawBuffers; i++) {
            if (any(targets & getTargetBufferFlagsAt(i))) {
                UpdateFrameBufferTexture(rt, color[i], GL_COLOR_ATTACHMENT0 + i);
            }
            bufs[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers((GLsizei)maxDrawBuffers, bufs);
    }

    bool specialCased = false;
    if ((targets & TargetBufferFlags::DEPTH_AND_STENCIL) == TargetBufferFlags::DEPTH_AND_STENCIL) {
        ET_CORE_ASSERT(!stencil.handle || stencil.handle == depth.handle);

        if (any(rt->depth->usage & TextureUsage::SAMPLEABLE) || (!depth.handle && !stencil.handle)) {
            // special case: depth & stencil requested, and both provided as the same texture
            // special case: depth & stencil requested, but both not provided
            specialCased = true;
            UpdateFrameBufferTexture(rt, depth, GL_DEPTH_STENCIL_ATTACHMENT);
        }
    }

    if (!specialCased) {
        if (any(targets & TargetBufferFlags::DEPTH)) {
            UpdateFrameBufferTexture(rt, depth, GL_DEPTH_ATTACHMENT);
        }
        if (any(targets & TargetBufferFlags::STENCIL)) {
            UpdateFrameBufferTexture(rt, stencil, GL_STENCIL_ATTACHMENT);
        }
    }
    return rt;
}

void OpenGLDriverApi::Draw(Ref<RenderPrimitive> rph, PipelineState pipeline) {
    Ref<GLRenderPrimitive> rp = rph.As<GLRenderPrimitive>();
    Ref<GLProgram> p = pipeline.program.As<GLProgram>();
    Ref<GLVertexBuffer> vb = rp->vertexBuffer.As<GLVertexBuffer>();
    Ref<GLIndexBuffer> ib = rp->indexBuffer.As<GLIndexBuffer>();

    glUseProgram(p->gl.id);
    uint32_t offset = 0;
    // TODO: multi sampler group binding will cause problem.
    // make sure only one sampler group binding ( must be 0 )is used for now.
    for (auto& sg : mSamplerGroupBindings) {
        if (!sg) continue;
        for (auto& entry : sg->entries) {
            Ref<GLTexture> tex = entry.texture.As<GLTexture>();
            if (!entry.texture || entry.binding == INVALID_UINT32) continue;

            if (entry.level == INVALID_UINT32 && entry.layer == INVALID_UINT32) {
                glActiveTexture(GL_TEXTURE0 + offset + entry.binding);
                glBindTexture(tex->gl.target, tex->gl.id);
                glBindSampler(offset + entry.binding, entry.id);
            } else {
                // TODO: this should be a image2d binding, not in samplergroup maybe
                // The first parameter is the index of the image unit to be modified, no need to add GL_TEXTURE0
                glBindImageTexture(offset + entry.binding, tex->gl.id, entry.level == INVALID_UINT32 ? 0 : entry.level,
                                   entry.layer == INVALID_UINT32 ? GL_FALSE : GL_TRUE, entry.layer == INVALID_UINT32 ? 0 : entry.layer, GL_READ_WRITE,
                                   GL_RGBA16F);
            }
        }
        offset += sg->entries.size();
    }

    glBindVertexArray(rp->gl.vao);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->gl.buffer);

    UpdateVertexArrayObject(rp, vb);

    SetRasterState(pipeline.rasterState);

    glDrawElementsBaseVertex(GLenum(rp->type), (GLsizei)rp->count, rp->gl.getIndicesType(), reinterpret_cast<const void*>(rp->offset), rp->minIndex);

    // glDrawRangeElements(GLenum(rp->type), rp->minIndex, rp->maxIndex, (GLsizei)rp->count, rp->gl.getIndicesType(),
    //                     reinterpret_cast<const void*>(rp->offset));
}

void OpenGLDriverApi::BeginRenderPass(RenderTargetHandle rth, const RenderPassParams& params) {
    mRenderPassTarget = rth;
    mRenderPassParams = params;
    Ref<GLRenderTarget> rt = rth.As<GLRenderTarget>();

    glBindFramebuffer(GL_FRAMEBUFFER, rt->gl.id);

    glClearColor(params.clearColor.x, params.clearColor.y, params.clearColor.z, params.clearColor.w);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glViewport(0, 0, rt->width, rt->height);
}

void OpenGLDriverApi::EndRenderPass() {
    Ref<GLRenderTarget> rt = mRenderPassTarget.As<GLRenderTarget>();
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void OpenGLDriverApi::SetVertexBufferObject(Ref<VertexBuffer> vbh, uint32_t index, Ref<BufferObject> boh) {
    Ref<GLVertexBuffer> vb = vbh.As<GLVertexBuffer>();
    Ref<GLBufferObject> bo = boh.As<GLBufferObject>();

    ET_CORE_ASSERT(bo->gl.binding == GL_ARRAY_BUFFER);
    if (vb->gl.buffers[index] != bo->gl.id) {
        vb->gl.buffers[index] = bo->gl.id;
    }
}

void OpenGLDriverApi::UpdateBufferObject(Ref<BufferObject> handle, BufferDescriptor&& bd, uint32_t byteOffset) {
    Ref<GLBufferObject> bo = handle.As<GLBufferObject>();

    // See CreateBufferObject()
    if (bo->gl.binding == GL_ARRAY_BUFFER) {
        glBindVertexArray(0);
    }

    glBindBuffer(bo->gl.binding, bo->gl.id);
    if (byteOffset == 0 && bd.size == bo->byteCount) {
        // it looks like it's generally faster (or not worse) to use glBufferData()
        glBufferData(bo->gl.binding, (GLsizeiptr)bd.size, bd.buffer, GLUtils::ResolveBufferUsage(bo->usage));
    } else {
        // glBufferSubData() could be catastrophically inefficient if several are
        // issued during the same frame. Currently, we're not doing that though.
        glBufferSubData(bo->gl.binding, byteOffset, (GLsizeiptr)bd.size, bd.buffer);
    }
}

void OpenGLDriverApi::UpdateIndexBuffer(Ref<IndexBuffer> handle, BufferDescriptor&& bd, uint32_t byteOffset) {
    Ref<GLIndexBuffer> ib = handle.As<GLIndexBuffer>();

    glBindVertexArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ib->gl.buffer);
    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, byteOffset, (GLsizeiptr)bd.size, bd.buffer);
}

void OpenGLDriverApi::SetTextureData(Ref<Texture> th, uint32_t levels, uint32_t xoffset, uint32_t yoffset, uint32_t zoffset, uint32_t width,
                                     uint32_t height, uint32_t depth, const PixelBufferDescriptor& desc) {
    GLenum format = GLUtils::ResolvePixelDataFormat(desc.dataFormat);
    GLenum type = GLUtils::ResolvePixelDataType(desc.dataType);
    Ref<GLTexture> t = th.As<GLTexture>();
    glPixelStorei(GL_UNPACK_ALIGNMENT, desc.alignment);
    glPixelStorei(GL_UNPACK_ROW_LENGTH, desc.stride);
    glPixelStorei(GL_UNPACK_SKIP_ROWS, desc.top);
    glPixelStorei(GL_UNPACK_SKIP_PIXELS, desc.left);

    glBindTexture(t->gl.target, t->gl.id);
    switch (t->gl.target) {
        case GL_TEXTURE_2D:
            glTexSubImage2D(t->gl.target, levels, xoffset, yoffset, width, height, format, type, desc.buffer);
            break;
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_3D:
            glTexSubImage3D(t->gl.target, levels, xoffset, yoffset, zoffset, width, height, depth, format, type, desc.buffer);
            break;
        case GL_TEXTURE_CUBE_MAP:
            ET_CORE_ASSERT(zoffset + depth <= 6)
            ET_CORE_ASSERT(width == height)
            const size_t faceSize =
                PixelBufferDescriptor::ComputeDataSize(desc.dataFormat, desc.dataType, desc.stride ? desc.stride : width, height, desc.alignment);
            for (size_t face = 0; face < depth; ++face) {
                glTexSubImage2D(t->gl.target, levels, xoffset, yoffset, width, height, format, type,
                                static_cast<uint8_t const*>(desc.buffer) + faceSize * face);
            }
            break;
    }
}

void OpenGLDriverApi::UpdateSamplerGroup(SamplerGroupHandle sgh, SamplerGroupDescriptor& desc) {
    Ref<GLSamplerGroup> sg = sgh.As<GLSamplerGroup>();
    ET_CORE_ASSERT(sg->entries.size() == desc.size());

    for (size_t i = 0; i < desc.size(); ++i) {
        auto& entry = sg->entries[i];
        if (desc[i].texture) {
            Ref<GLTexture> t = desc[i].texture.As<GLTexture>();
            entry.texture = t;
            entry.binding = desc[i].binding;
            entry.level = desc[i].level;
            entry.layer = desc[i].layer;

            auto& params = desc[i].params;

            if (!entry.id) glGenSamplers(1, &entry.id);
            glSamplerParameteri(entry.id, GL_TEXTURE_MIN_FILTER, (GLint)GLUtils::ResolveSamplerMinFilter(params.filterMin));
            glSamplerParameteri(entry.id, GL_TEXTURE_MAG_FILTER, (GLint)GLUtils::ResolveSamplerMagFilter(params.filterMag));
            glSamplerParameteri(entry.id, GL_TEXTURE_WRAP_S, (GLint)GLUtils::ResolveSamplerWrapMode(params.wrapS));
            glSamplerParameteri(entry.id, GL_TEXTURE_WRAP_T, (GLint)GLUtils::ResolveSamplerWrapMode(params.wrapT));
            glSamplerParameteri(entry.id, GL_TEXTURE_WRAP_R, (GLint)GLUtils::ResolveSamplerWrapMode(params.wrapR));
            glSamplerParameteri(entry.id, GL_TEXTURE_COMPARE_MODE, (GLint)GLUtils::ResolveSamplerCompareMode(params.compareMode));
            glSamplerParameteri(entry.id, GL_TEXTURE_COMPARE_FUNC, (GLint)GLUtils::ResolveSamplerCompareFunc(params.compareFunc));
        }
    }
}

void OpenGLDriverApi::BindSamplerGroup(uint8_t binding, Ref<SamplerGroup> sgh) { mSamplerGroupBindings[binding] = sgh.As<GLSamplerGroup>(); }

void OpenGLDriverApi::BindUniformBuffer(uint8_t binding, BufferObjectHandle boh) {
    Ref<GLBufferObject> bo = boh.As<GLBufferObject>();
    ET_CORE_ASSERT(bo->gl.binding = GL_UNIFORM_BUFFER, "BufferObjectHandle's binding point must be GL_UNIFORM_BUFFER");
    glBindBufferRange(bo->gl.binding, GLuint(binding), bo->gl.id, 0, bo->byteCount);
}

uint32_t OpenGLDriverApi::GetTextueID(TextureHandle th) {
    Ref<GLTexture> t = th.As<GLTexture>();
    return t->gl.id;
}

void OpenGLDriverApi::GetSubTexture(TextureHandle th, uint32_t layer, TextureHandle dsth) {
    Ref<GLTexture> t = th.As<GLTexture>();
    Ref<GLTexture> dst = dsth.As<GLTexture>();

    ET_CORE_ASSERT(dst->width == t->width && dst->height == t->height, "dst texture must has same size with src texture");
    glCopyImageSubData(t->gl.id, t->gl.target, 0, 0, 0, layer, dst->gl.id, GL_TEXTURE_2D, 0, 0, 0, 0, t->width, t->height, 1);
}

int OpenGLDriverApi::ReadPixel(RenderTargetHandle rth, uint32_t attachmentIndex, uint32_t xoffset, uint32_t yoffset) {
    Ref<GLRenderTarget> rt = rth.As<GLRenderTarget>();
    glBindFramebuffer(GL_FRAMEBUFFER, rt->gl.id);

    glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
    int pixelData;
    glReadPixels(xoffset, yoffset, 1, 1, GL_RED_INTEGER, GL_INT, &pixelData);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return pixelData;
}

void OpenGLDriverApi::Clear() { glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); }

void OpenGLDriverApi::GenerateMipmaps(TextureHandle th) {
    Ref<GLTexture> t = th.As<GLTexture>();
    glBindTexture(t->gl.target, t->gl.id);

    t->gl.baseLevel = 0;
    t->gl.maxLevel = static_cast<uint8_t>(t->levels - 1);

    glTexParameteri(t->gl.target, GL_TEXTURE_BASE_LEVEL, t->gl.baseLevel);
    glTexParameteri(t->gl.target, GL_TEXTURE_MAX_LEVEL, t->gl.maxLevel);

    glGenerateMipmap(t->gl.target);
    glBindTexture(t->gl.target, 0);
}

void OpenGLDriverApi::SetRenderTargetAttachment(RenderTargetHandle rth, TargetBufferInfo const& info, TargetBufferFlags flag) {
    Ref<RenderTarget> rt = rth.As<RenderTarget>();
    // This will resize the render target
    auto valueForLevel = [](size_t level, size_t value) { return std::max(size_t(1), value >> level); };
    rt->width = valueForLevel(info.level, info.handle->width);
    rt->height = valueForLevel(info.level, info.handle->height);

    if (any(flag & TargetBufferFlags::COLOR_ALL)) {
        const size_t maxDrawBuffers = MAX_SUPPORTED_RENDER_TARGET_COUNT;
        for (size_t i = 0; i < maxDrawBuffers; i++) {
            if (any(flag & getTargetBufferFlagsAt(i))) {
                UpdateFrameBufferTexture(rt, info, GL_COLOR_ATTACHMENT0 + i);
            }
        }
    }
    if ((flag & TargetBufferFlags::DEPTH_AND_STENCIL) == TargetBufferFlags::DEPTH_AND_STENCIL) {
        ET_CORE_ASSERT(false, "Not supported update stencil&depth framebuffer yet");
    }
    if (any(flag & TargetBufferFlags::DEPTH)) {
        UpdateFrameBufferTexture(rt, info, GL_DEPTH_ATTACHMENT);
    }

    if (any(flag & TargetBufferFlags::STENCIL)) {
        UpdateFrameBufferTexture(rt, info, GL_STENCIL_ATTACHMENT);
    }
}

//---------------------------------------------------------------------
// Private functions below
//---------------------------------------------------------------------
void OpenGLDriverApi::AllocateTexture(Ref<GLTexture> t, uint32_t width, uint32_t height, uint32_t depth) {
    // Note: glTexStorage only specify the memory for texture, no actual data are provided.
    // Also allow reallocate the memory for the texture, like resizing.
    glBindTexture(t->gl.target, t->gl.id);
    switch (t->gl.target) {
        case GL_TEXTURE_2D:
        case GL_TEXTURE_CUBE_MAP:
            glTexStorage2D(t->gl.target, t->levels, t->gl.format, width, height);
            break;
        case GL_TEXTURE_3D:
        case GL_TEXTURE_2D_ARRAY:
            glTexStorage3D(t->gl.target, t->levels, t->gl.format, width, height, depth);
    };
    t->width = width;
    t->height = height;
    t->depth = depth;
}

void OpenGLDriverApi::UpdateVertexArrayObject(Ref<GLRenderPrimitive> rp, Ref<GLVertexBuffer> vb) {
    rp->maxVertexCount = vb->vertexCount;
    for (size_t i = 0; i < vb->attributes.size(); ++i) {
        const auto& attribute = vb->attributes[i];
        const uint8_t bi = attribute.buffer;

        if (bi != Attribute::BUFFER_UNUSED && vb->gl.buffers[bi] != 0) {
            glBindBuffer(GL_ARRAY_BUFFER, vb->gl.buffers[bi]);

            if (attribute.flags & Attribute::FLAG_INTEGER_TARGET) {
                glVertexAttribIPointer(GLuint(i), (GLint)GLUtils::ResolveElementTypeCount(attribute.type),
                                       GLUtils::ResolveElementType(attribute.type), attribute.stride, (void*)uintptr_t(attribute.offset));
            } else {
                glVertexAttribPointer(GLuint(i), (GLint)GLUtils::ResolveElementTypeCount(attribute.type), GLUtils::ResolveElementType(attribute.type),
                                      GLUtils::ResolveNormalize(attribute.flags & Attribute::FLAG_NORMALIZED), attribute.stride,
                                      (void*)uintptr_t(attribute.offset));
            }

            glEnableVertexAttribArray(GLuint(i));
        } else {
            // In some OpenGL implementations, we must supply a properly-typed placeholder for
            // every integer input that is declared in the vertex shader.
            if (attribute.flags & Attribute::FLAG_INTEGER_TARGET) {
                glVertexAttribI4ui(GLuint(i), 0, 0, 0, 0);
            } else {
                glVertexAttrib4f(GLuint(i), 0, 0, 0, 0);
            }

            glDisableVertexAttribArray(GLuint(i));
        }
    }
}

void OpenGLDriverApi::UpdateFrameBufferTexture(Ref<GLRenderTarget> rt, TargetBufferInfo const& info, GLenum attachment) {
    if (!info.handle) return;  // this attachment is as a placeholder

    Ref<GLTexture> t = info.handle.As<GLTexture>();

    auto valueForLevel = [](size_t level, size_t value) { return std::max(size_t(1), value >> level); };
    ET_CORE_ASSERT((rt->width <= valueForLevel(info.level, t->width) && rt->height <= valueForLevel(info.level, t->height)),
                   "Invalid size of attachment!");
    TargetBufferFlags resolveFlags = {};
    switch (attachment) {
        case GL_COLOR_ATTACHMENT0:
        case GL_COLOR_ATTACHMENT1:
        case GL_COLOR_ATTACHMENT2:
        case GL_COLOR_ATTACHMENT3:
        case GL_COLOR_ATTACHMENT4:
        case GL_COLOR_ATTACHMENT5:
        case GL_COLOR_ATTACHMENT6:
        case GL_COLOR_ATTACHMENT7: {
            static_assert(MAX_SUPPORTED_RENDER_TARGET_COUNT == 8);
            resolveFlags = getTargetBufferFlagsAt(attachment - GL_COLOR_ATTACHMENT0);
            rt->color[attachment - GL_COLOR_ATTACHMENT0] = info.handle;
            break;
        }
        case GL_DEPTH_ATTACHMENT: {
            resolveFlags = TargetBufferFlags::DEPTH;
            rt->depth = info.handle;
            break;
        }
        case GL_STENCIL_ATTACHMENT: {
            resolveFlags = TargetBufferFlags::STENCIL;
            rt->stencil = info.handle;
            break;
        }
        case GL_DEPTH_STENCIL_ATTACHMENT: {
            resolveFlags = TargetBufferFlags::DEPTH;
            resolveFlags |= TargetBufferFlags::STENCIL;
            rt->depth = info.handle;
            rt->stencil = info.handle;
            break;
        }
        default:
            break;
    }

    GLenum target = GL_TEXTURE_2D;
    if (any(t->usage & TextureUsage::SAMPLEABLE)) {
        target = t->gl.target;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, rt->gl.id);
    switch (target) {
        case GL_TEXTURE_CUBE_MAP:
            ET_CORE_ASSERT(info.layer >= 0 && info.layer < 6, "Invalid layer for cubemap!");
            glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, GL_TEXTURE_CUBE_MAP_POSITIVE_X + info.layer, t->gl.id, info.level);
            break;
        case GL_TEXTURE_2D:
        case GL_TEXTURE_2D_MULTISAMPLE:
            if (any(t->usage & TextureUsage::SAMPLEABLE)) {
                glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, t->gl.id, info.level);
            } else {
                ET_CORE_ASSERT(target == GL_TEXTURE_2D);
                glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, t->gl.id);
            }
            break;
        case GL_TEXTURE_2D_ARRAY:
        case GL_TEXTURE_CUBE_MAP_ARRAY: {
            if (info.layer == INVALID_UINT32)
                glFramebufferTexture(GL_FRAMEBUFFER, attachment, t->gl.id, info.level);  // multi layer render
            else
                glFramebufferTextureLayer(GL_FRAMEBUFFER, attachment, t->gl.id, info.level, info.layer);
            break;
        }
        default:
            // we shouldn't be here
            break;
    }

    if (any(t->usage & TextureUsage::SAMPLEABLE)) {
        // In a sense, drawing to a texture level is similar to calling setTextureData on it; in
        // both cases, we update the base/max LOD to give shaders access to levels as they become
        // available.  Note that this can only expand the LOD range (never shrink it), and that
        // users can override this range by calling setMinMaxLevels().

        // updateTextureLodRange(t, (int8_t)binfo.level);
    }

    auto fboStatus = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    ET_CORE_ASSERT(fboStatus == GL_FRAMEBUFFER_COMPLETE, "Framebuffer is incomplete!");
}

void OpenGLDriverApi::SetRasterState(RasterState state) {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    if (state.EnableDepthTest)
        glEnable(GL_DEPTH_TEST);
    else
        glDisable(GL_DEPTH_TEST);

    if (state.EnableDepthWrite)
        glDepthMask(GL_TRUE);
    else
        glDepthMask(GL_FALSE);
    glDepthFunc(GLUtils::ResolveDepthFunc(state.depthFunc));
}

}  // namespace Backend
}  // namespace Ethereal