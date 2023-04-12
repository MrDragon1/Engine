#pragma once
#include "Platform/Windows/Backend/BufferDescriptor.h"

namespace Ethereal {
template <typename T, size_t N = 1>
class TypedUniform {
   public:
    T& ItemAt(size_t i) noexcept {
        mSomethingDirty = true;
        return mBuffer[i];
    }

    T& Edit() noexcept { return ItemAt(0); }

    // size of the uniform buffer in bytes
    size_t GetSize() const noexcept { return sizeof(T) * N; }

    // return if any uniform has been changed
    bool IsDirty() const noexcept { return mSomethingDirty; }

    // mark the whole buffer as "clean" (no modified uniforms)
    void Clean() const noexcept { mSomethingDirty = false; }

    // helper functions

    BufferDescriptor ToBufferDescriptor() const noexcept { return ToBufferDescriptor(0, GetSize()); }

    // copy the UBO data and cleans the dirty bits
    BufferDescriptor ToBufferDescriptor(size_t offset, size_t size) const noexcept {
        BufferDescriptor p;
        p.size = size;
        p.buffer = (void*)new char[p.size];
        memcpy(p.buffer, reinterpret_cast<const char*>(mBuffer) + offset, p.size);
        Clean();
        return p;
    }

   private:
    T mBuffer[N];
    mutable bool mSomethingDirty = false;
};

}  // namespace Ethereal