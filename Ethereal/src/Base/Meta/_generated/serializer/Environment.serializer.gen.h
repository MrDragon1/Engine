#pragma once
#include "Resource/Descriptor/Environment.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const EnvironmentDesc& instance);
    template <>
        EnvironmentDesc& Serializer::read(const YNode& yaml_context, EnvironmentDesc & instance);
}  // namespace Ethereal

