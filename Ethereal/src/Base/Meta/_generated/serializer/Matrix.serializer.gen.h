#pragma once
#include "Base/Math/Matrix.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Matrix4x4_& instance);
    template <>
        Matrix4x4_& Serializer::read(const YNode& yaml_context, Matrix4x4_ & instance);
}  // namespace Ethereal

