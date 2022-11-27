#pragma once
#include "Base/Meta/Raw/Camera.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const CameraRaw& instance);
    template <>
        CameraRaw& Serializer::read(const YNode& yaml_context, CameraRaw & instance);
}  // namespace Ethereal

