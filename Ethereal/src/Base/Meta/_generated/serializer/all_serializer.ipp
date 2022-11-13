#pragma once
#include "Base/Meta/_generated/serializer/Matrix.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Quaternion.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Vector.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Transform.serializer.gen.h"
namespace Ethereal
{
    template <>
    YNode Serializer::write(const Matrix4x4_ & instance) {
        YNode ret_context;
        ret_context["v0"] = Serializer::write(instance.v0);
        ret_context["v1"] = Serializer::write(instance.v1);
        ret_context["v2"] = Serializer::write(instance.v2);
        ret_context["v3"] = Serializer::write(instance.v3);
        ret_context["v4"] = Serializer::write(instance.v4);
        ret_context["v5"] = Serializer::write(instance.v5);
        ret_context["v6"] = Serializer::write(instance.v6);
        ret_context["v7"] = Serializer::write(instance.v7);
        ret_context["v8"] = Serializer::write(instance.v8);
        ret_context["v9"] = Serializer::write(instance.v9);
        ret_context["v10"] = Serializer::write(instance.v10);
        ret_context["v11"] = Serializer::write(instance.v11);
        ret_context["v12"] = Serializer::write(instance.v12);
        ret_context["v13"] = Serializer::write(instance.v13);
        ret_context["v14"] = Serializer::write(instance.v14);
        ret_context["v15"] = Serializer::write(instance.v15);
        return ret_context;
    }
    template <>
    Matrix4x4_&Serializer::read(const YNode& yaml_context, Matrix4x4_&instance) {
        assert(!yaml_context.IsNull());
        if (!yaml_context["v0"].IsNull()) {
            Serializer::read(yaml_context["v0"], instance.v0);
        }
        if (!yaml_context["v1"].IsNull()) {
            Serializer::read(yaml_context["v1"], instance.v1);
        }
        if (!yaml_context["v2"].IsNull()) {
            Serializer::read(yaml_context["v2"], instance.v2);
        }
        if (!yaml_context["v3"].IsNull()) {
            Serializer::read(yaml_context["v3"], instance.v3);
        }
        if (!yaml_context["v4"].IsNull()) {
            Serializer::read(yaml_context["v4"], instance.v4);
        }
        if (!yaml_context["v5"].IsNull()) {
            Serializer::read(yaml_context["v5"], instance.v5);
        }
        if (!yaml_context["v6"].IsNull()) {
            Serializer::read(yaml_context["v6"], instance.v6);
        }
        if (!yaml_context["v7"].IsNull()) {
            Serializer::read(yaml_context["v7"], instance.v7);
        }
        if (!yaml_context["v8"].IsNull()) {
            Serializer::read(yaml_context["v8"], instance.v8);
        }
        if (!yaml_context["v9"].IsNull()) {
            Serializer::read(yaml_context["v9"], instance.v9);
        }
        if (!yaml_context["v10"].IsNull()) {
            Serializer::read(yaml_context["v10"], instance.v10);
        }
        if (!yaml_context["v11"].IsNull()) {
            Serializer::read(yaml_context["v11"], instance.v11);
        }
        if (!yaml_context["v12"].IsNull()) {
            Serializer::read(yaml_context["v12"], instance.v12);
        }
        if (!yaml_context["v13"].IsNull()) {
            Serializer::read(yaml_context["v13"], instance.v13);
        }
        if (!yaml_context["v14"].IsNull()) {
            Serializer::read(yaml_context["v14"], instance.v14);
        }
        if (!yaml_context["v15"].IsNull()) {
            Serializer::read(yaml_context["v15"], instance.v15);
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

