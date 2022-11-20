#pragma once
#include "Resource/Descriptor/Mesh.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const StaticMeshDesc& instance);
    template <>
        StaticMeshDesc& Serializer::read(const YNode& yaml_context, StaticMeshDesc & instance);
    template <>
    YNode Serializer::write(const MeshDesc& instance);
    template <>
        MeshDesc& Serializer::read(const YNode& yaml_context, MeshDesc & instance);
}  // namespace Ethereal

