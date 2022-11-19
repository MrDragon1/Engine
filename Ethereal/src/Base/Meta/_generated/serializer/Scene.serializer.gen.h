#pragma once
#include "Base/Meta/Raw/Scene.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const EntityRaw& instance);
    template <>
        EntityRaw& Serializer::read(const YNode& yaml_context, EntityRaw & instance);
    template <>
    YNode Serializer::write(const SceneRaw& instance);
    template <>
        SceneRaw& Serializer::read(const YNode& yaml_context, SceneRaw & instance);
}  // namespace Ethereal

