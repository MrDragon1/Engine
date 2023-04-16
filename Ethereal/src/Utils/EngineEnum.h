#pragma once
#include <assert.h>
#include <stddef.h>
#include <stdint.h>

namespace Ethereal {
//* UI stuff *//
enum class ETHEREAL_BASIC_3DOBJECT : uint8_t {
    ETHEREAL_BASIC_3DOBJECT_CUBE = 0,
    ETHEREAL_BASIC_3DOBJECT_SPHERE,
    ETHEREAL_BASIC_3DOBJECT_CYLINDER,
    ETHEREAL_BASIC_3DOBJECT_CONE,
    ETHEREAL_BASIC_3DOBJECT_TORUS,
    ETHEREAL_BASIC_3DOBJECT_PLANE,
    ETHEREAL_BASIC_3DOBJECT_QUAD,
    ETHEREAL_BASIC_3DOBJECT_MONKEY
};

enum class MaterialPinType : uint32_t {
    Unknown = BIT(0),
    Bool = BIT(1),
    Float = BIT(2),
    RGB = BIT(3),
    BSDF = BIT(4),
    Texture = BIT(5),
};
enum class PinKind : uint8_t {
    Input,
    Output,
};

enum class NodeType : uint8_t {
    BluePrint,
    Comment,
};

}  // namespace Ethereal