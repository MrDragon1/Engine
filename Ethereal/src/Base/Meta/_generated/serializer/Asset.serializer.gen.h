#pragma once
#include "Core/Asset/Asset.h"
#include "Base/Meta/_generated/serializer/Ref.serializer.gen.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Asset& instance);
    template <>
        Asset& Serializer::read(const YNode& yaml_context, Asset & instance);
}  // namespace Ethereal

