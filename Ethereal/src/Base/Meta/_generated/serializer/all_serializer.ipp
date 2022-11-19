#pragma once
#include "Base/Meta/_generated/serializer/Components.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Quaternion.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Mesh.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Vector.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Matrix.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Transform.serializer.gen.h"
#include "Base/Meta/_generated/serializer/UUID.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Camera.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Scene.serializer.gen.h"
namespace Ethereal
{
    template <>
    YNode Serializer::write(const Component& instance) {
        YNode ret_context;
         
        return ret_context;
    }
    template <>
    Component&Serializer::read(const YNode& yaml_context, Component& instance) {
        assert(yaml_context.IsDefined());
         
        return instance;
    }
    template <>
    YNode Serializer::write(const IDComponent& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Component*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["ID"] = Serializer::write(instance.ID);
        return ret_context;
    }
    template <>
    IDComponent&Serializer::read(const YNode& yaml_context, IDComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["ID"].IsNull()) {
            Serializer::read(yaml_context["ID"], instance.ID);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const TagComponent& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Component*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["Tag"] = Serializer::write(instance.Tag);
        return ret_context;
    }
    template <>
    TagComponent&Serializer::read(const YNode& yaml_context, TagComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["Tag"].IsNull()) {
            Serializer::read(yaml_context["Tag"], instance.Tag);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const TransformComponent& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Component*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["Position"] = Serializer::write(instance.Position);
        ret_context["Scale"] = Serializer::write(instance.Scale);
        ret_context["Rotation"] = Serializer::write(instance.Rotation);
        return ret_context;
    }
    template <>
    TransformComponent&Serializer::read(const YNode& yaml_context, TransformComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["Position"].IsNull()) {
            Serializer::read(yaml_context["Position"], instance.Position);
        }
        if (!yaml_context["Scale"].IsNull()) {
            Serializer::read(yaml_context["Scale"], instance.Scale);
        }
        if (!yaml_context["Rotation"].IsNull()) {
            Serializer::read(yaml_context["Rotation"], instance.Rotation);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const StaticMeshComponent& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Component*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["StaticMeshHandle"] = Serializer::write(instance.StaticMeshHandle);
        ret_context["MaterialTableRaw"] = Serializer::write(instance.MaterialTableRaw);
        return ret_context;
    }
    template <>
    StaticMeshComponent&Serializer::read(const YNode& yaml_context, StaticMeshComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["StaticMeshHandle"].IsNull()) {
            Serializer::read(yaml_context["StaticMeshHandle"], instance.StaticMeshHandle);
        }
        if (!yaml_context["MaterialTableRaw"].IsNull()) {
            Serializer::read(yaml_context["MaterialTableRaw"], instance.MaterialTableRaw);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const MeshComponent& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Component*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["MeshHandle"] = Serializer::write(instance.MeshHandle);
        return ret_context;
    }
    template <>
    MeshComponent&Serializer::read(const YNode& yaml_context, MeshComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["MeshHandle"].IsNull()) {
            Serializer::read(yaml_context["MeshHandle"], instance.MeshHandle);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const CameraComponent& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Component*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["Camera"] = Serializer::write(instance.Camera);
        return ret_context;
    }
    template <>
    CameraComponent&Serializer::read(const YNode& yaml_context, CameraComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["Camera"].IsNull()) {
            Serializer::read(yaml_context["Camera"], instance.Camera);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Quaternion& instance) {
        YNode ret_context;
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        ret_context["z"] = Serializer::write(instance.z);
        ret_context["w"] = Serializer::write(instance.w);
        return ret_context;
    }
    template <>
    Quaternion&Serializer::read(const YNode& yaml_context, Quaternion& instance) {
        assert(yaml_context.IsDefined());
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
    YNode Serializer::write(const MaterialTableRaw& instance) {
        YNode ret_context;
        YNode Materials_yaml;
        for (auto& item : instance.Materials) {
        Materials_yaml.push_back(Serializer::write(item));
        }
        ret_context["Materials"] = Materials_yaml;
        return ret_context;
    }
    template <>
    MaterialTableRaw&Serializer::read(const YNode& yaml_context, MaterialTableRaw& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Materials"].IsNull()) {
            assert(yaml_context["Materials"].IsSequence());
            YNode array_Materials = yaml_context["Materials"];
            instance.Materials.resize(array_Materials.size());
            for (size_t index = 0; index < array_Materials.size(); ++index) {
                Serializer::read(array_Materials[index], instance.Materials[index]);
            }
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Vector2& instance) {
        YNode ret_context;
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        return ret_context;
    }
    template <>
    Vector2&Serializer::read(const YNode& yaml_context, Vector2& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["x"].IsNull()) {
            Serializer::read(yaml_context["x"], instance.x);
        }
        if (!yaml_context["y"].IsNull()) {
            Serializer::read(yaml_context["y"], instance.y);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const Vector3& instance) {
        YNode ret_context;
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        ret_context["z"] = Serializer::write(instance.z);
        return ret_context;
    }
    template <>
    Vector3&Serializer::read(const YNode& yaml_context, Vector3& instance) {
        assert(yaml_context.IsDefined());
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
    YNode Serializer::write(const Vector4& instance) {
        YNode ret_context;
        ret_context["x"] = Serializer::write(instance.x);
        ret_context["y"] = Serializer::write(instance.y);
        ret_context["z"] = Serializer::write(instance.z);
        ret_context["w"] = Serializer::write(instance.w);
        return ret_context;
    }
    template <>
    Vector4&Serializer::read(const YNode& yaml_context, Vector4& instance) {
        assert(yaml_context.IsDefined());
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
    YNode Serializer::write(const Matrix3& instance) {
        YNode ret_context;
        ret_context["value0"] = Serializer::write(instance.value0);
        ret_context["value1"] = Serializer::write(instance.value1);
        ret_context["value2"] = Serializer::write(instance.value2);
        return ret_context;
    }
    template <>
    Matrix3&Serializer::read(const YNode& yaml_context, Matrix3& instance) {
        assert(yaml_context.IsDefined());
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
    YNode Serializer::write(const Matrix4& instance) {
        YNode ret_context;
        ret_context["value0"] = Serializer::write(instance.value0);
        ret_context["value1"] = Serializer::write(instance.value1);
        ret_context["value2"] = Serializer::write(instance.value2);
        ret_context["value3"] = Serializer::write(instance.value3);
        return ret_context;
    }
    template <>
    Matrix4&Serializer::read(const YNode& yaml_context, Matrix4& instance) {
        assert(yaml_context.IsDefined());
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
    YNode Serializer::write(const Transform& instance) {
        YNode ret_context;
        ret_context["position"] = Serializer::write(instance.m_position);
        ret_context["scale"] = Serializer::write(instance.m_scale);
        ret_context["rotation"] = Serializer::write(instance.m_rotation);
        return ret_context;
    }
    template <>
    Transform&Serializer::read(const YNode& yaml_context, Transform& instance) {
        assert(yaml_context.IsDefined());
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
    template <>
    YNode Serializer::write(const UUID& instance) {
        YNode ret_context;
        ret_context["UUID"] = Serializer::write(instance.m_UUID);
        return ret_context;
    }
    template <>
    UUID&Serializer::read(const YNode& yaml_context, UUID& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["UUID"].IsNull()) {
            Serializer::read(yaml_context["UUID"], instance.m_UUID);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const CameraRaw& instance) {
        YNode ret_context;
        ret_context["Primary"] = Serializer::write(instance.Primary);
        ret_context["FixedAspectRatio"] = Serializer::write(instance.FixedAspectRatio);
        ret_context["Perspective"] = Serializer::write(instance.Perspective);
        ret_context["Fov"] = Serializer::write(instance.Fov);
        ret_context["Near"] = Serializer::write(instance.Near);
        ret_context["Far"] = Serializer::write(instance.Far);
        ret_context["AspectRatio"] = Serializer::write(instance.AspectRatio);
        ret_context["OrthographicSize"] = Serializer::write(instance.OrthographicSize);
        ret_context["OrthographicNear"] = Serializer::write(instance.OrthographicNear);
        ret_context["OrthographicFar"] = Serializer::write(instance.OrthographicFar);
        return ret_context;
    }
    template <>
    CameraRaw&Serializer::read(const YNode& yaml_context, CameraRaw& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Primary"].IsNull()) {
            Serializer::read(yaml_context["Primary"], instance.Primary);
        }
        if (!yaml_context["FixedAspectRatio"].IsNull()) {
            Serializer::read(yaml_context["FixedAspectRatio"], instance.FixedAspectRatio);
        }
        if (!yaml_context["Perspective"].IsNull()) {
            Serializer::read(yaml_context["Perspective"], instance.Perspective);
        }
        if (!yaml_context["Fov"].IsNull()) {
            Serializer::read(yaml_context["Fov"], instance.Fov);
        }
        if (!yaml_context["Near"].IsNull()) {
            Serializer::read(yaml_context["Near"], instance.Near);
        }
        if (!yaml_context["Far"].IsNull()) {
            Serializer::read(yaml_context["Far"], instance.Far);
        }
        if (!yaml_context["AspectRatio"].IsNull()) {
            Serializer::read(yaml_context["AspectRatio"], instance.AspectRatio);
        }
        if (!yaml_context["OrthographicSize"].IsNull()) {
            Serializer::read(yaml_context["OrthographicSize"], instance.OrthographicSize);
        }
        if (!yaml_context["OrthographicNear"].IsNull()) {
            Serializer::read(yaml_context["OrthographicNear"], instance.OrthographicNear);
        }
        if (!yaml_context["OrthographicFar"].IsNull()) {
            Serializer::read(yaml_context["OrthographicFar"], instance.OrthographicFar);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const EntityRaw& instance) {
        YNode ret_context;
        ret_context["Name"] = Serializer::write(instance.Name);
        YNode Components_yaml;
        for (auto& item : instance.Components) {
        Components_yaml.push_back(Serializer::write(item));
        }
        ret_context["Components"] = Components_yaml;
        return ret_context;
    }
    template <>
    EntityRaw&Serializer::read(const YNode& yaml_context, EntityRaw& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Name"].IsNull()) {
            Serializer::read(yaml_context["Name"], instance.Name);
        }
        if (!yaml_context["Components"].IsNull()) {
            assert(yaml_context["Components"].IsSequence());
            YNode array_Components = yaml_context["Components"];
            instance.Components.resize(array_Components.size());
            for (size_t index = 0; index < array_Components.size(); ++index) {
                Serializer::read(array_Components[index], instance.Components[index]);
            }
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const SceneRaw& instance) {
        YNode ret_context;
        ret_context["Name"] = Serializer::write(instance.Name);
        YNode Entities_yaml;
        for (auto& item : instance.Entities) {
        Entities_yaml.push_back(Serializer::write(item));
        }
        ret_context["Entities"] = Entities_yaml;
        return ret_context;
    }
    template <>
    SceneRaw&Serializer::read(const YNode& yaml_context, SceneRaw& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Name"].IsNull()) {
            Serializer::read(yaml_context["Name"], instance.Name);
        }
        if (!yaml_context["Entities"].IsNull()) {
            assert(yaml_context["Entities"].IsSequence());
            YNode array_Entities = yaml_context["Entities"];
            instance.Entities.resize(array_Entities.size());
            for (size_t index = 0; index < array_Entities.size(); ++index) {
                Serializer::read(array_Entities[index], instance.Entities[index]);
            }
        }
        return instance;
    }
}  // namespace Ethereal

