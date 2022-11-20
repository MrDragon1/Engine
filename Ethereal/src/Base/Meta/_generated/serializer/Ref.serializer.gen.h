#pragma once
#include "Base/Ref.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const RefCounted& instance);
    template <>
        RefCounted& Serializer::read(const YNode& yaml_context, RefCounted & instance);
}  // namespace Ethereal

