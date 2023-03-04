#pragma once
#include "DriverEnums.h"

namespace Ethereal {
namespace Backend {
class BufferDescriptor {
   public:
    BufferDescriptor(void* buffer, size_t size) : buffer(buffer), size(size) {}

    size_t size = 0;
    void* buffer;

   private:
};

class PixelBufferDescriptor final : public BufferDescriptor {
   public:
    PixelBufferDescriptor(void* buffer, size_t size, PixelDataFormat format, PixelDataType type, uint32_t alignment = 1, uint32_t left = 0,
                          uint32_t top = 0, uint32_t stride = 0)
        : BufferDescriptor(buffer, size), left(left), top(top), stride(stride), alignment(alignment) {}

    static size_t ComputeDataSize(PixelDataFormat format, PixelDataType type, uint32_t stride, uint32_t height, uint32_t alignment);

    uint32_t left;
    uint32_t top;
    uint32_t stride;
    uint32_t alignment;

    PixelDataFormat dataFormat;
    PixelDataType dataType;
};

}  // namespace Backend
}  // namespace Ethereal