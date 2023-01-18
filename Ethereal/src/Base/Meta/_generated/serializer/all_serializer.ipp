#pragma once
#include "Base/Meta/_generated/serializer/Quaternion.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Vector.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Material.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Matrix.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Ref.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Transform.serializer.gen.h"
#include "Base/Meta/_generated/serializer/UUID.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Asset.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Animation.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Mesh.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Camera.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Components.serializer.gen.h"
#include "Base/Meta/_generated/serializer/Scene.serializer.gen.h"
namespace Ethereal
{
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
    YNode Serializer::write(const MaterialDesc& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Asset*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["AlbedoMap"] = Serializer::write(instance.AlbedoMap);
        ret_context["NormalMap"] = Serializer::write(instance.NormalMap);
        ret_context["MetallicMap"] = Serializer::write(instance.MetallicMap);
        ret_context["RoughnessMap"] = Serializer::write(instance.RoughnessMap);
        ret_context["OcclusionMap"] = Serializer::write(instance.OcclusionMap);
        ret_context["Albedo"] = Serializer::write(instance.Albedo);
        ret_context["Metallic"] = Serializer::write(instance.Metallic);
        ret_context["Roughness"] = Serializer::write(instance.Roughness);
        ret_context["Emisstion"] = Serializer::write(instance.Emisstion);
        ret_context["Transparency"] = Serializer::write(instance.Transparency);
        ret_context["IsAlbedo"] = Serializer::write(instance.IsAlbedo);
        ret_context["IsMetallic"] = Serializer::write(instance.IsMetallic);
        ret_context["IsRoughness"] = Serializer::write(instance.IsRoughness);
        ret_context["IsNormal"] = Serializer::write(instance.IsNormal);
        ret_context["IsOcclusion"] = Serializer::write(instance.IsOcclusion);
        ret_context["IsTransparent"] = Serializer::write(instance.IsTransparent);
        ret_context["Name"] = Serializer::write(instance.Name);
        return ret_context;
    }
    template <>
    MaterialDesc&Serializer::read(const YNode& yaml_context, MaterialDesc& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Asset*)&instance);
        if (!yaml_context["AlbedoMap"].IsNull()) {
            Serializer::read(yaml_context["AlbedoMap"], instance.AlbedoMap);
        }
        if (!yaml_context["NormalMap"].IsNull()) {
            Serializer::read(yaml_context["NormalMap"], instance.NormalMap);
        }
        if (!yaml_context["MetallicMap"].IsNull()) {
            Serializer::read(yaml_context["MetallicMap"], instance.MetallicMap);
        }
        if (!yaml_context["RoughnessMap"].IsNull()) {
            Serializer::read(yaml_context["RoughnessMap"], instance.RoughnessMap);
        }
        if (!yaml_context["OcclusionMap"].IsNull()) {
            Serializer::read(yaml_context["OcclusionMap"], instance.OcclusionMap);
        }
        if (!yaml_context["Albedo"].IsNull()) {
            Serializer::read(yaml_context["Albedo"], instance.Albedo);
        }
        if (!yaml_context["Metallic"].IsNull()) {
            Serializer::read(yaml_context["Metallic"], instance.Metallic);
        }
        if (!yaml_context["Roughness"].IsNull()) {
            Serializer::read(yaml_context["Roughness"], instance.Roughness);
        }
        if (!yaml_context["Emisstion"].IsNull()) {
            Serializer::read(yaml_context["Emisstion"], instance.Emisstion);
        }
        if (!yaml_context["Transparency"].IsNull()) {
            Serializer::read(yaml_context["Transparency"], instance.Transparency);
        }
        if (!yaml_context["IsAlbedo"].IsNull()) {
            Serializer::read(yaml_context["IsAlbedo"], instance.IsAlbedo);
        }
        if (!yaml_context["IsMetallic"].IsNull()) {
            Serializer::read(yaml_context["IsMetallic"], instance.IsMetallic);
        }
        if (!yaml_context["IsRoughness"].IsNull()) {
            Serializer::read(yaml_context["IsRoughness"], instance.IsRoughness);
        }
        if (!yaml_context["IsNormal"].IsNull()) {
            Serializer::read(yaml_context["IsNormal"], instance.IsNormal);
        }
        if (!yaml_context["IsOcclusion"].IsNull()) {
            Serializer::read(yaml_context["IsOcclusion"], instance.IsOcclusion);
        }
        if (!yaml_context["IsTransparent"].IsNull()) {
            Serializer::read(yaml_context["IsTransparent"], instance.IsTransparent);
        }
        if (!yaml_context["Name"].IsNull()) {
            Serializer::read(yaml_context["Name"], instance.Name);
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
    YNode Serializer::write(const RefCounted& instance) {
        YNode ret_context;
         
        return ret_context;
    }
    template <>
    RefCounted&Serializer::read(const YNode& yaml_context, RefCounted& instance) {
        assert(yaml_context.IsDefined());
         
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
    YNode Serializer::write(const Asset& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::RefCounted*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        return ret_context;
    }
    template <>
    Asset&Serializer::read(const YNode& yaml_context, Asset& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::RefCounted*)&instance);
        return instance;
    }
    template <>
    YNode Serializer::write(const AnimPositionState& instance) {
        YNode ret_context;
        ret_context["Position"] = Serializer::write(instance.Position);
        ret_context["TimeStamp"] = Serializer::write(instance.TimeStamp);
        return ret_context;
    }
    template <>
    AnimPositionState&Serializer::read(const YNode& yaml_context, AnimPositionState& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Position"].IsNull()) {
            Serializer::read(yaml_context["Position"], instance.Position);
        }
        if (!yaml_context["TimeStamp"].IsNull()) {
            Serializer::read(yaml_context["TimeStamp"], instance.TimeStamp);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const AnimRotationState& instance) {
        YNode ret_context;
        ret_context["Rotation"] = Serializer::write(instance.Rotation);
        ret_context["TimeStamp"] = Serializer::write(instance.TimeStamp);
        return ret_context;
    }
    template <>
    AnimRotationState&Serializer::read(const YNode& yaml_context, AnimRotationState& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Rotation"].IsNull()) {
            Serializer::read(yaml_context["Rotation"], instance.Rotation);
        }
        if (!yaml_context["TimeStamp"].IsNull()) {
            Serializer::read(yaml_context["TimeStamp"], instance.TimeStamp);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const AnimScaleState& instance) {
        YNode ret_context;
        ret_context["Scale"] = Serializer::write(instance.Scale);
        ret_context["TimeStamp"] = Serializer::write(instance.TimeStamp);
        return ret_context;
    }
    template <>
    AnimScaleState&Serializer::read(const YNode& yaml_context, AnimScaleState& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Scale"].IsNull()) {
            Serializer::read(yaml_context["Scale"], instance.Scale);
        }
        if (!yaml_context["TimeStamp"].IsNull()) {
            Serializer::read(yaml_context["TimeStamp"], instance.TimeStamp);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const AnimKeyClip& instance) {
        YNode ret_context;
        YNode PositionStates_yaml;
        for (auto& item : instance.PositionStates) {
        PositionStates_yaml.push_back(Serializer::write(item));
        }
        ret_context["PositionStates"] = PositionStates_yaml;
        YNode RotationStates_yaml;
        for (auto& item : instance.RotationStates) {
        RotationStates_yaml.push_back(Serializer::write(item));
        }
        ret_context["RotationStates"] = RotationStates_yaml;
        YNode ScaleStates_yaml;
        for (auto& item : instance.ScaleStates) {
        ScaleStates_yaml.push_back(Serializer::write(item));
        }
        ret_context["ScaleStates"] = ScaleStates_yaml;
        ret_context["JointID"] = Serializer::write(instance.JointID);
        return ret_context;
    }
    template <>
    AnimKeyClip&Serializer::read(const YNode& yaml_context, AnimKeyClip& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["PositionStates"].IsNull()) {
            assert(yaml_context["PositionStates"].IsSequence());
            YNode array_PositionStates = yaml_context["PositionStates"];
            instance.PositionStates.resize(array_PositionStates.size());
            for (size_t index = 0; index < array_PositionStates.size(); ++index) {
                Serializer::read(array_PositionStates[index], instance.PositionStates[index]);
            }
        }
        if (!yaml_context["RotationStates"].IsNull()) {
            assert(yaml_context["RotationStates"].IsSequence());
            YNode array_RotationStates = yaml_context["RotationStates"];
            instance.RotationStates.resize(array_RotationStates.size());
            for (size_t index = 0; index < array_RotationStates.size(); ++index) {
                Serializer::read(array_RotationStates[index], instance.RotationStates[index]);
            }
        }
        if (!yaml_context["ScaleStates"].IsNull()) {
            assert(yaml_context["ScaleStates"].IsSequence());
            YNode array_ScaleStates = yaml_context["ScaleStates"];
            instance.ScaleStates.resize(array_ScaleStates.size());
            for (size_t index = 0; index < array_ScaleStates.size(); ++index) {
                Serializer::read(array_ScaleStates[index], instance.ScaleStates[index]);
            }
        }
        if (!yaml_context["JointID"].IsNull()) {
            Serializer::read(yaml_context["JointID"], instance.JointID);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const AnimState& instance) {
        YNode ret_context;
        ret_context["PositionState"] = Serializer::write(instance.PositionState);
        ret_context["RotationState"] = Serializer::write(instance.RotationState);
        ret_context["ScaleState"] = Serializer::write(instance.ScaleState);
        ret_context["JointID"] = Serializer::write(instance.JointID);
        return ret_context;
    }
    template <>
    AnimState&Serializer::read(const YNode& yaml_context, AnimState& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["PositionState"].IsNull()) {
            Serializer::read(yaml_context["PositionState"], instance.PositionState);
        }
        if (!yaml_context["RotationState"].IsNull()) {
            Serializer::read(yaml_context["RotationState"], instance.RotationState);
        }
        if (!yaml_context["ScaleState"].IsNull()) {
            Serializer::read(yaml_context["ScaleState"], instance.ScaleState);
        }
        if (!yaml_context["JointID"].IsNull()) {
            Serializer::read(yaml_context["JointID"], instance.JointID);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const AnimationDesc& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Asset*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        YNode Clips_yaml;
        for (auto& item : instance.Clips) {
        Clips_yaml.push_back(Serializer::write(item));
        }
        ret_context["Clips"] = Clips_yaml;
        ret_context["FramesPersecond"] = Serializer::write(instance.FramesPersecond);
        ret_context["Duration"] = Serializer::write(instance.Duration);
        ret_context["Name"] = Serializer::write(instance.Name);
        return ret_context;
    }
    template <>
    AnimationDesc&Serializer::read(const YNode& yaml_context, AnimationDesc& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Asset*)&instance);
        if (!yaml_context["Clips"].IsNull()) {
            assert(yaml_context["Clips"].IsSequence());
            YNode array_Clips = yaml_context["Clips"];
            instance.Clips.resize(array_Clips.size());
            for (size_t index = 0; index < array_Clips.size(); ++index) {
                Serializer::read(array_Clips[index], instance.Clips[index]);
            }
        }
        if (!yaml_context["FramesPersecond"].IsNull()) {
            Serializer::read(yaml_context["FramesPersecond"], instance.FramesPersecond);
        }
        if (!yaml_context["Duration"].IsNull()) {
            Serializer::read(yaml_context["Duration"], instance.Duration);
        }
        if (!yaml_context["Name"].IsNull()) {
            Serializer::read(yaml_context["Name"], instance.Name);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const AnimatorDesc& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Asset*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["Animation"] = Serializer::write(instance.Animation);
        ret_context["Skeleton"] = Serializer::write(instance.Skeleton);
        return ret_context;
    }
    template <>
    AnimatorDesc&Serializer::read(const YNode& yaml_context, AnimatorDesc& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Asset*)&instance);
        if (!yaml_context["Animation"].IsNull()) {
            Serializer::read(yaml_context["Animation"], instance.Animation);
        }
        if (!yaml_context["Skeleton"].IsNull()) {
            Serializer::read(yaml_context["Skeleton"], instance.Skeleton);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const JointDesc& instance) {
        YNode ret_context;
        YNode Children_yaml;
        for (auto& item : instance.Children) {
        Children_yaml.push_back(Serializer::write(item));
        }
        ret_context["Children"] = Children_yaml;
        ret_context["Name"] = Serializer::write(instance.Name);
        ret_context["ID"] = Serializer::write(instance.ID);
        ret_context["InitialPosition"] = Serializer::write(instance.InitialPosition);
        ret_context["InitialRotation"] = Serializer::write(instance.InitialRotation);
        ret_context["InitialScale"] = Serializer::write(instance.InitialScale);
        ret_context["OffsetMatrix"] = Serializer::write(instance.OffsetMatrix);
        ret_context["InverseOffsetMatrix"] = Serializer::write(instance.InverseOffsetMatrix);
        ret_context["LocalTransform"] = Serializer::write(instance.LocalTransform);
        return ret_context;
    }
    template <>
    JointDesc&Serializer::read(const YNode& yaml_context, JointDesc& instance) {
        assert(yaml_context.IsDefined());
        if (!yaml_context["Children"].IsNull()) {
            assert(yaml_context["Children"].IsSequence());
            YNode array_Children = yaml_context["Children"];
            instance.Children.resize(array_Children.size());
            for (size_t index = 0; index < array_Children.size(); ++index) {
                Serializer::read(array_Children[index], instance.Children[index]);
            }
        }
        if (!yaml_context["Name"].IsNull()) {
            Serializer::read(yaml_context["Name"], instance.Name);
        }
        if (!yaml_context["ID"].IsNull()) {
            Serializer::read(yaml_context["ID"], instance.ID);
        }
        if (!yaml_context["InitialPosition"].IsNull()) {
            Serializer::read(yaml_context["InitialPosition"], instance.InitialPosition);
        }
        if (!yaml_context["InitialRotation"].IsNull()) {
            Serializer::read(yaml_context["InitialRotation"], instance.InitialRotation);
        }
        if (!yaml_context["InitialScale"].IsNull()) {
            Serializer::read(yaml_context["InitialScale"], instance.InitialScale);
        }
        if (!yaml_context["OffsetMatrix"].IsNull()) {
            Serializer::read(yaml_context["OffsetMatrix"], instance.OffsetMatrix);
        }
        if (!yaml_context["InverseOffsetMatrix"].IsNull()) {
            Serializer::read(yaml_context["InverseOffsetMatrix"], instance.InverseOffsetMatrix);
        }
        if (!yaml_context["LocalTransform"].IsNull()) {
            Serializer::read(yaml_context["LocalTransform"], instance.LocalTransform);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const SkeletonDesc& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Asset*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["RootJoint"] = Serializer::write(instance.RootJoint);
        ret_context["Name"] = Serializer::write(instance.Name);
        return ret_context;
    }
    template <>
    SkeletonDesc&Serializer::read(const YNode& yaml_context, SkeletonDesc& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Asset*)&instance);
        if (!yaml_context["RootJoint"].IsNull()) {
            Serializer::read(yaml_context["RootJoint"], instance.RootJoint);
        }
        if (!yaml_context["Name"].IsNull()) {
            Serializer::read(yaml_context["Name"], instance.Name);
        }
        return instance;
    }
    template <>
    YNode Serializer::write(const StaticMeshDesc& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Asset*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["Mesh"] = Serializer::write(instance.Mesh);
        YNode Materials_yaml;
        for (auto& item : instance.Materials) {
        Materials_yaml.push_back(Serializer::write(item));
        }
        ret_context["Materials"] = Materials_yaml;
        return ret_context;
    }
    template <>
    StaticMeshDesc&Serializer::read(const YNode& yaml_context, StaticMeshDesc& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Asset*)&instance);
        if (!yaml_context["Mesh"].IsNull()) {
            Serializer::read(yaml_context["Mesh"], instance.Mesh);
        }
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
    YNode Serializer::write(const MeshDesc& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Asset*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["Mesh"] = Serializer::write(instance.Mesh);
        YNode Materials_yaml;
        for (auto& item : instance.Materials) {
        Materials_yaml.push_back(Serializer::write(item));
        }
        ret_context["Materials"] = Materials_yaml;
        ret_context["Animator"] = Serializer::write(instance.Animator);
        return ret_context;
    }
    template <>
    MeshDesc&Serializer::read(const YNode& yaml_context, MeshDesc& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Asset*)&instance);
        if (!yaml_context["Mesh"].IsNull()) {
            Serializer::read(yaml_context["Mesh"], instance.Mesh);
        }
        if (!yaml_context["Materials"].IsNull()) {
            assert(yaml_context["Materials"].IsSequence());
            YNode array_Materials = yaml_context["Materials"];
            instance.Materials.resize(array_Materials.size());
            for (size_t index = 0; index < array_Materials.size(); ++index) {
                Serializer::read(array_Materials[index], instance.Materials[index]);
            }
        }
        if (!yaml_context["Animator"].IsNull()) {
            Serializer::read(yaml_context["Animator"], instance.Animator);
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
    YNode Serializer::write(const BasicPropertyComponent& instance) {
        YNode ret_context;
        auto&& yaml_context_0 = Serializer::write(*(Ethereal::Component*)&instance);
        assert(yaml_context_0.IsDefined());
        ret_context = yaml_context_0;
        ret_context["Tag"] = Serializer::write(instance.Tag);
        ret_context["Visible"] = Serializer::write(instance.Visible);
        return ret_context;
    }
    template <>
    BasicPropertyComponent&Serializer::read(const YNode& yaml_context, BasicPropertyComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["Tag"].IsNull()) {
            Serializer::read(yaml_context["Tag"], instance.Tag);
        }
        if (!yaml_context["Visible"].IsNull()) {
            Serializer::read(yaml_context["Visible"], instance.Visible);
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
        ret_context["MaterialTableRaw"] = Serializer::write(instance.MaterialTableRaw);
        return ret_context;
    }
    template <>
    MeshComponent&Serializer::read(const YNode& yaml_context, MeshComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["MeshHandle"].IsNull()) {
            Serializer::read(yaml_context["MeshHandle"], instance.MeshHandle);
        }
        if (!yaml_context["MaterialTableRaw"].IsNull()) {
            Serializer::read(yaml_context["MaterialTableRaw"], instance.MaterialTableRaw);
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
        ret_context["Primary"] = Serializer::write(instance.Primary);
        return ret_context;
    }
    template <>
    CameraComponent&Serializer::read(const YNode& yaml_context, CameraComponent& instance) {
        assert(yaml_context.IsDefined());
        Serializer::read(yaml_context, *(Ethereal::Component*)&instance);
        if (!yaml_context["Camera"].IsNull()) {
            Serializer::read(yaml_context["Camera"], instance.Camera);
        }
        if (!yaml_context["Primary"].IsNull()) {
            Serializer::read(yaml_context["Primary"], instance.Primary);
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
        ret_context["Environment"] = Serializer::write(instance.Environment);
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
        if (!yaml_context["Environment"].IsNull()) {
            Serializer::read(yaml_context["Environment"], instance.Environment);
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

