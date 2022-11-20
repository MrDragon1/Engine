#pragma once
#include "Resource/Descriptor/Animation.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const AnimPositionState& instance);
    template <>
        AnimPositionState& Serializer::read(const YNode& yaml_context, AnimPositionState & instance);
    template <>
    YNode Serializer::write(const AnimRotationState& instance);
    template <>
        AnimRotationState& Serializer::read(const YNode& yaml_context, AnimRotationState & instance);
    template <>
    YNode Serializer::write(const AnimScaleState& instance);
    template <>
        AnimScaleState& Serializer::read(const YNode& yaml_context, AnimScaleState & instance);
    template <>
    YNode Serializer::write(const AnimKeyClip& instance);
    template <>
        AnimKeyClip& Serializer::read(const YNode& yaml_context, AnimKeyClip & instance);
    template <>
    YNode Serializer::write(const AnimState& instance);
    template <>
        AnimState& Serializer::read(const YNode& yaml_context, AnimState & instance);
    template <>
    YNode Serializer::write(const AnimationDesc& instance);
    template <>
        AnimationDesc& Serializer::read(const YNode& yaml_context, AnimationDesc & instance);
    template <>
    YNode Serializer::write(const AnimatorDesc& instance);
    template <>
        AnimatorDesc& Serializer::read(const YNode& yaml_context, AnimatorDesc & instance);
    template <>
    YNode Serializer::write(const JointDesc& instance);
    template <>
        JointDesc& Serializer::read(const YNode& yaml_context, JointDesc & instance);
    template <>
    YNode Serializer::write(const SkeletonDesc& instance);
    template <>
        SkeletonDesc& Serializer::read(const YNode& yaml_context, SkeletonDesc & instance);
}  // namespace Ethereal

