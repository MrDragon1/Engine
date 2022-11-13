#pragma once
#include "Base/Math/Transform.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Transform& instance);
    template <>
        Transform& Serializer::read(const YNode& yaml_context, Transform & instance);
}  // namespace Ethereal

