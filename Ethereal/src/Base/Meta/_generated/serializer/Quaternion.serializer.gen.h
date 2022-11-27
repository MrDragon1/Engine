#pragma once
#include "Base/Math/Quaternion.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Quaternion& instance);
    template <>
        Quaternion& Serializer::read(const YNode& yaml_context, Quaternion & instance);
}  // namespace Ethereal

