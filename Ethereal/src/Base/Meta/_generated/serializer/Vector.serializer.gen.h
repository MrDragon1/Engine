#pragma once
#include "Base/Math/Vector.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Vector2& instance);
    template <>
        Vector2& Serializer::read(const YNode& yaml_context, Vector2 & instance);
    template <>
    YNode Serializer::write(const Vector3& instance);
    template <>
        Vector3& Serializer::read(const YNode& yaml_context, Vector3 & instance);
    template <>
    YNode Serializer::write(const Vector4& instance);
    template <>
        Vector4& Serializer::read(const YNode& yaml_context, Vector4 & instance);
}  // namespace Ethereal

