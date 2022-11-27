#pragma once
#include "Base/Math/Matrix.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Matrix3& instance);
    template <>
        Matrix3& Serializer::read(const YNode& yaml_context, Matrix3 & instance);
    template <>
    YNode Serializer::write(const Matrix4& instance);
    template <>
        Matrix4& Serializer::read(const YNode& yaml_context, Matrix4 & instance);
}  // namespace Ethereal

