#pragma once
#include "Resource/Descriptor/Material.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const MaterialTableRaw& instance);
    template <>
        MaterialTableRaw& Serializer::read(const YNode& yaml_context, MaterialTableRaw & instance);
    template <>
    YNode Serializer::write(const MaterialDesc& instance);
    template <>
        MaterialDesc& Serializer::read(const YNode& yaml_context, MaterialDesc & instance);
}  // namespace Ethereal

