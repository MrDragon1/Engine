#pragma once
#include "Core/Scene/Components.h"

namespace Ethereal
{
    template <>
    YNode Serializer::write(const Component& instance);
    template <>
        Component& Serializer::read(const YNode& yaml_context, Component & instance);
    template <>
    YNode Serializer::write(const IDComponent& instance);
    template <>
        IDComponent& Serializer::read(const YNode& yaml_context, IDComponent & instance);
    template <>
    YNode Serializer::write(const BasicPropertyComponent& instance);
    template <>
        BasicPropertyComponent& Serializer::read(const YNode& yaml_context, BasicPropertyComponent & instance);
    template <>
    YNode Serializer::write(const TransformComponent& instance);
    template <>
        TransformComponent& Serializer::read(const YNode& yaml_context, TransformComponent & instance);
    template <>
    YNode Serializer::write(const StaticMeshComponent& instance);
    template <>
        StaticMeshComponent& Serializer::read(const YNode& yaml_context, StaticMeshComponent & instance);
    template <>
    YNode Serializer::write(const MeshComponent& instance);
    template <>
        MeshComponent& Serializer::read(const YNode& yaml_context, MeshComponent & instance);
    template <>
    YNode Serializer::write(const CameraComponent& instance);
    template <>
        CameraComponent& Serializer::read(const YNode& yaml_context, CameraComponent & instance);
}  // namespace Ethereal

