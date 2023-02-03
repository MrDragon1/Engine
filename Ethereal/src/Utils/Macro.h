#pragma once
//#include "pch.h"

namespace Ethereal
{
    enum class ETHEREAL_SHADER_TYPE : uint8_t
    {
        VERTEX = 0,
        FRAGMENT,
        GEOMETRY,
    };

    enum class ETHEREAL_PIXEL_FORMAT : uint8_t
    {
        UNKNOWN = 0,
        R8G8B8_UNORM,
        R8G8B8_SRGB,
        R8G8B8A8_UNORM,
        R8G8B8A8_SRGB,
        R32G32_FLOAT,
        R32G32B32_FLOAT,
        R32G32B32A32_FLOAT,
        R32_INTEGER,
        DEPTH,
        RED,
        R16G16B16A16_HDR,
        PLACEHOLDER,
    };

    enum class ETHEREAL_FILTER_FORMAT : uint8_t
    {
        ETHEREAL_FILTER_FORMAT_LINEAR = 0,
        ETHEREAL_FILTER_FORMAT_NEAREST,
        ETHEREAL_FILTER_FORMAT_NEAREST_MIPMAP_NEAREST,
        ETHEREAL_FILTER_FORMAT_LINEAR_MIPMAP_NEAREST,
        ETHEREAL_FILTER_FORMAT_NEAREST_MIPMAP_LINEAR,
        ETHEREAL_FILTER_FORMAT_LINEAR_MIPMAP_LINEAR,
    };

    enum class ETHEREAL_WARP_FORMAT : uint8_t
    {
        ETHEREAL_WARP_FORMAT_CLAMP_TO_EDGE = 0,
        ETHEREAL_WARP_FORMAT_CLAMP_TO_BORDER,
        ETHEREAL_WARP_FORMAT_REPEAT,
    };

    enum class ETHEREAL_IMAGE_TYPE : uint8_t
    {
        ETHEREAL_IMAGE_TYPE_UNKNOWM = 0,
        ETHEREAL_IMAGE_TYPE_2D,
        ETHEREAL_IMAGE_TYPE_2D_ARRAY
    };

    enum class ETHEREAL_DRAW_MODE : uint8_t
    {
        ETHEREAL_DRAW_MODE_FILLED = 0,
        ETHEREAL_DRAW_MODE_LINE,
        ETHEREAL_DRAW_MODE_POINT
    };

    enum class ETHEREAL_CULLFACE_TYPE
    {
        FRONT = 0,
        BACK,
    };

    enum class ETHEREAL_DEPTH_FUNC
    {
        NEVER = 0,
        LESS,
        EQUAL,
        LEQUAL,
        GREATER,
        NOTEQUAL,
        GEQUAL,
        ALWAYS
    };

    //* UI stuff *//
    enum class ETHEREAL_BASIC_3DOBJECT : uint8_t
    {
        ETHEREAL_BASIC_3DOBJECT_CUBE = 0,
        ETHEREAL_BASIC_3DOBJECT_SPHERE,
        ETHEREAL_BASIC_3DOBJECT_CYLINDER,
        ETHEREAL_BASIC_3DOBJECT_CONE,
        ETHEREAL_BASIC_3DOBJECT_TORUS,
        ETHEREAL_BASIC_3DOBJECT_PLANE,
        ETHEREAL_BASIC_3DOBJECT_QUAD,
        ETHEREAL_BASIC_3DOBJECT_MONKEY
    };
}  // namespace Ethereal