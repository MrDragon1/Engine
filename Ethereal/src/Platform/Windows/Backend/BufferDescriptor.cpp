#include "BufferDescriptor.h"

namespace Ethereal {

namespace Backend {

size_t PixelBufferDescriptor::ComputeDataSize(PixelDataFormat format, PixelDataType type, uint32_t stride, uint32_t height, uint32_t alignment) {
    size_t n = 0;
    switch (format) {
        case PixelDataFormat::R:
        case PixelDataFormat::RI:
        case PixelDataFormat::ALPHA:
        case PixelDataFormat::DEPTH:
            n = 1;
            break;
        case PixelDataFormat::RG:
        case PixelDataFormat::RGI:
        case PixelDataFormat::DEPTH_STENCIL:
            n = 2;
            break;
        case PixelDataFormat::RGB:
        case PixelDataFormat::RGBI:
            n = 3;
            break;
        case PixelDataFormat::RGBA:
        case PixelDataFormat::RGBAI:
            n = 4;
            break;
    }

    size_t bpp = n;
    switch (type) {
        case PixelDataType::BYTE:
        case PixelDataType::UBYTE:
            break;
        case PixelDataType::SHORT:
        case PixelDataType::USHORT:
            bpp *= 2;
            break;
        case PixelDataType::INT:
        case PixelDataType::UINT:
        case PixelDataType::FLOAT:
            bpp *= 4;
            break;
    }

    size_t bpr = bpp * stride;
    size_t bprAligned = (bpr + (alignment - 1)) & (~alignment + 1);
    return bprAligned * height;
}

}  // namespace Backend
}  // namespace Ethereal