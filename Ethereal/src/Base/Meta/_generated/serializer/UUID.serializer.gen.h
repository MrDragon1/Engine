#pragma once
#include "Base/UUID.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const UUID& instance);
    template <>
        UUID& Serializer::read(const YNode& yaml_context, UUID & instance);
}  // namespace Ethereal

