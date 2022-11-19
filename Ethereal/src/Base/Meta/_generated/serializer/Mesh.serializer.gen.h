#pragma once
#include "Base/Meta/Raw/Mesh.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const MaterialTableRaw& instance);
    template <>
        MaterialTableRaw& Serializer::read(const YNode& yaml_context, MaterialTableRaw & instance);
}  // namespace Ethereal

