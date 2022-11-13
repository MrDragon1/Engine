#pragma once
#include "Base/Math/Vector.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Vector2& instance);
    template <>
    Vector2& Serializer::read(const YNode& yaml_context, Vector2& instance);
}  // namespace Ethereal
