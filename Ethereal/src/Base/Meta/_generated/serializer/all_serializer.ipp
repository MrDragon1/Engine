#pragma once
#include "Base/Meta/_generated/serializer/Matrix.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Quaternion.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Vector.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Transform.serializer.gen.h"
namespace Ethereal
{
    template <>
    YNode Serializer::write(const Matrix3 & instance) {
        YNode ret_context;
        ret_context["value0"] = Serializer::write(instance.value0);
        ret_context["value1"] = Serializer::write(instance.value1);
        ret_context["value2"] = Serializer::write(instance.value2);
        return ret_context;
    }
    template <>
    Matrix3&Serializer::read(const YNode& yaml_context, Matrix3&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["value0"].IsNull()) {
            Serializer::read(yaml_context["value0"], instance.value0);
        }
        if (!yaml_context["value1"].IsNull()) {
            Serializer::read(yaml_context["value1"], instance.value1);
        }
        if (!yaml_context["value2"].IsNull()) {
            Serializer::read(yaml_context["value2"], instance.value2);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Matrix4 & instance) {
        YNode ret_context;
        ret_context["value0"] = Serializer::write(instance.value0);
        ret_context["value1"] = Serializer::write(instance.value1);
        ret_context["value2"] = Serializer::write(instance.value2);
        ret_context["value3"] = Serializer::write(instance.value3);
        return ret_context;
    }
    template <>
    Matrix4&Serializer::read(const YNode& yaml_context, Matrix4&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["value0"].IsNull()) {
            Serializer::read(yaml_context["value0"], instance.value0);
        }
        if (!yaml_context["value1"].IsNull()) {
            Serializer::read(yaml_context["value1"], instance.value1);
        }
        if (!yaml_context["value2"].IsNull()) {
            Serializer::read(yaml_context["value2"], instance.value2);
        }
        if (!yaml_context["value3"].IsNull()) {
            Serializer::read(yaml_context["value3"], instance.value3);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Quaternion & instance) {
        YNode ret_context;
        ret_context["w"] = Serializer::write(instance.w);
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        ret_context["z"] = Serializer::write(instance.z);
        return ret_context;
    }
    template <>
    Quaternion&Serializer::read(const YNode& yaml_context, Quaternion&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["w"].IsNull()) {
            Serializer::read(yaml_context["w"], instance.w);
        }
        if (!yaml_context["x"].IsNull()) {
            Serializer::read(yaml_context["x"], instance.x);
        }
        if (!yaml_context["y"].IsNull()) {
            Serializer::read(yaml_context["y"], instance.y);
        }
        if (!yaml_context["z"].IsNull()) {
            Serializer::read(yaml_context["z"], instance.z);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Vector2 & instance) {
        YNode ret_context;
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        return ret_context;
    }
    template <>
    Vector2&Serializer::read(const YNode& yaml_context, Vector2&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["x"].IsNull()) {
            Serializer::read(yaml_context["x"], instance.x);
        }
        if (!yaml_context["y"].IsNull()) {
            Serializer::read(yaml_context["y"], instance.y);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Vector3 & instance) {
        YNode ret_context;
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        ret_context["z"] = Serializer::write(instance.z);
        return ret_context;
    }
    template <>
    Vector3&Serializer::read(const YNode& yaml_context, Vector3&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["x"].IsNull()) {
            Serializer::read(yaml_context["x"], instance.x);
        }
        if (!yaml_context["y"].IsNull()) {
            Serializer::read(yaml_context["y"], instance.y);
        }
        if (!yaml_context["z"].IsNull()) {
            Serializer::read(yaml_context["z"], instance.z);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Vector4 & instance) {
        YNode ret_context;
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        ret_context["z"] = Serializer::write(instance.z);
        ret_context["w"] = Serializer::write(instance.w);
        return ret_context;
    }
    template <>
    Vector4&Serializer::read(const YNode& yaml_context, Vector4&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["x"].IsNull()) {
            Serializer::read(yaml_context["x"], instance.x);
        }
        if (!yaml_context["y"].IsNull()) {
            Serializer::read(yaml_context["y"], instance.y);
        }
        if (!yaml_context["z"].IsNull()) {
            Serializer::read(yaml_context["z"], instance.z);
        }
        if (!yaml_context["w"].IsNull()) {
            Serializer::read(yaml_context["w"], instance.w);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Transform & instance) {
        YNode ret_context;
        ret_context["position"] = Serializer::write(instance.m_position);
        ret_context["scale"] = Serializer::write(instance.m_scale);
        ret_context["rotation"] = Serializer::write(instance.m_rotation);
        return ret_context;
    }
    template <>
    Transform&Serializer::read(const YNode& yaml_context, Transform&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["position"].IsNull()) {
            Serializer::read(yaml_context["position"], instance.m_position);
        }
        if (!yaml_context["scale"].IsNull()) {
            Serializer::read(yaml_context["scale"], instance.m_scale);
        }
        if (!yaml_context["rotation"].IsNull()) {
            Serializer::read(yaml_context["rotation"], instance.m_rotation);
        }
        return instance;
    }
}  // namespace Ethereal

