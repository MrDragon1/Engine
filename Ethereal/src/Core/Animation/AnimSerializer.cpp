#include "AnimSerializer.h"
#include "Utils/YAMLSerializationHelpers.h"
#include "Core/Asset/AssetManager.h"
#include <fstream>
#include <filesystem>
#include "Base/Yaml.h"

namespace Ethereal
{
    void AnimSerializer::Serialize(const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Animation";
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << m_Animation->m_Name;
        out << YAML::Key << "AssetHandle" << YAML::Value << m_Animation->Handle;
        out << YAML::Key << "FramesPersecond" << YAML::Value << m_Animation->m_FramesPersecond;
        out << YAML::EndMap;

        out << YAML::Key << "KeyFrames" << YAML::Value << YAML::BeginSeq;

        for (auto& kf : m_Animation->m_KeyClips) {
            out << YAML::BeginMap;
            out << YAML::Key << "JointID" << YAML::Value << kf.JointID;

            out << YAML::Key << "PositionStates" << YAML::Value;
            out << YAML::BeginSeq;
            for (auto& pos : kf.PositionStates) {
                out << YAML::BeginMap;
                out << YAML::Key << "Position" << YAML::Value << pos.Position;
                out << YAML::Key << "TimeStamp" << YAML::Value << pos.TimeStamp;
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;

            out << YAML::Key << "RotationStates" << YAML::Value;
            out << YAML::BeginSeq;
            for (auto& rot : kf.RotationStates) {
                out << YAML::BeginMap;
                out << YAML::Key << "Rotation" << YAML::Value << rot.Rotation;
                out << YAML::Key << "TimeStamp" << YAML::Value << rot.TimeStamp;
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;

            out << YAML::Key << "ScaleStates" << YAML::Value;
            out << YAML::BeginSeq;
            for (auto& scale : kf.ScaleStates) {
                out << YAML::BeginMap;
                out << YAML::Key << "Scale" << YAML::Value << scale.Scale;
                out << YAML::Key << "TimeStamp" << YAML::Value << scale.TimeStamp;
                out << YAML::EndMap;
            }
            out << YAML::EndSeq;

            out << YAML::EndMap;
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool AnimSerializer::Deserialize(const std::string& filepath) {
        YNode data;
        try {
            data = YAML::LoadFile(filepath);
        } catch (YAML::ParserException& e) {
            ET_CORE_ERROR("Failed to load Animation file '{0}'\n     {1}", filepath, e.what());
            return false;
        }
        if (!data["Animation"]) return false;

        m_Animation->m_Name = data["Animation"]["Name"].as<std::string>();
        m_Animation->m_FramesPersecond = data["Animation"]["FramesPersecond"].as<size_t>();

        auto keyFrames = data["KeyFrames"];
        if (keyFrames) {
            for (auto kf : keyFrames) {
                AnimKeyClip keyClip;
                keyClip.JointID = kf["JointID"].as<size_t>();
                auto keyStates = kf["States"];

                auto positionStates = keyStates["PositionStates"];
                for (auto pos : positionStates) {
                    AnimPositionState state{};
                    state.Position = pos["Position"].as<Vector3>();
                    state.TimeStamp = pos["TimeStamp"].as<float>();
                    keyClip.PositionStates.push_back(state);
                }

                auto rotationStates = keyStates["RotationStates"];
                for (auto rot : rotationStates) {
                    AnimRotationState state{};
                    state.Rotation = rot["Rotation"].as<Quaternion>();
                    state.TimeStamp = rot["TimeStamp"].as<float>();
                    keyClip.RotationStates.push_back(state);
                }

                auto scaleStates = keyStates["ScaleStates"];
                for (auto scale : positionStates) {
                    AnimScaleState state{};
                    state.Scale = scale["Scale"].as<Vector3>();
                    state.TimeStamp = scale["TimeStamp"].as<float>();
                    keyClip.ScaleStates.push_back(state);
                }

                m_Animation->m_KeyClips.push_back(keyClip);
            }
        }
        return true;
    }

    void SkelSerializer::Serialize(const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Skeleton";
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << m_Skeleton->m_Name;
        out << YAML::Key << "AssetHandle" << YAML::Value << m_Skeleton->Handle;
        out << YAML::EndMap;
        out << YAML::Key << "Joints" << YAML::Value << YAML::BeginSeq;

        SerializeJoint(m_Skeleton->m_Root, out);

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool SkelSerializer::Deserialize(const std::string& filepath) {
        YNode data;
        try {
            data = YAML::LoadFile(filepath);
        } catch (YAML::ParserException& e) {
            ET_CORE_ERROR("Failed to load Skeleton file '{0}'\n     {1}", filepath, e.what());
            return false;
        }
        if (!data["Skeleton"]) return false;

        m_Skeleton->m_Name = data["Skeleton"]["Name"].as<std::string>();
        m_Skeleton->m_JointsMap.clear();
        m_Skeleton->m_NameIDMap.clear();

        auto joints = data["Joints"];
        if (joints) {
            for (const auto& joint : joints) {
                m_Skeleton->m_Root = DeserializeJoint(joint);
                break;
            }
        }
        return true;
    }

    void SkelSerializer::SerializeJoint(Ref<Joint> node, YAML::Emitter& out) {
        out << YAML::BeginMap;
        out << YAML::Key << "m_Name" << YAML::Value << node->m_Name;
        out << YAML::Key << "m_ID" << YAML::Value << node->m_ID;
        out << YAML::Key << "m_InitialPosition" << YAML::Value << node->m_InitialPosition;
        out << YAML::Key << "m_InitialRotation" << YAML::Value << node->m_InitialRotation;
        out << YAML::Key << "m_InitialScale" << YAML::Value << node->m_InitialScale;
        // InverseT can calculate with 3 InitialPoses

        out << YAML::Key << "Children" << YAML::Value << YAML::BeginSeq;
        for (auto& child : node->m_Children) SerializeJoint(child, out);
        out << YAML::EndSeq;

        out << YAML::EndMap;
    }

    Ref<Joint> SkelSerializer::DeserializeJoint(const YNode& node) {
        Ref<Joint> joint = Ref<Joint>::Create();
        joint->m_Name = node["m_Name"].as<std::string>();
        joint->m_ID = node["m_ID"].as<AssetHandle>();
        joint->m_InitialPosition = node["m_InitialPosition"].as<Vector3>();
        joint->m_InitialRotation = node["m_InitialRotation"].as<Quaternion>();
        joint->m_InitialScale = node["m_InitialScale"].as<Vector3>();
        // TODO: calculate inverseT

        joint->m_Children.clear();

        m_Skeleton->m_NameIDMap[joint->m_Name] = joint->m_ID;
        m_Skeleton->m_JointsMap[joint->m_ID] = joint;

        auto children = node["Children"];
        for (auto child : children) {
            joint->m_Children.emplace_back(DeserializeJoint(child));
        }
        return joint;
    }

    void AnimatorSerializer::Serialize(const std::string& filepath) {
        std::filesystem::path fp = filepath;
        std::string path = fp.parent_path().string() + "\\" + fp.stem().string();

        auto animationAsset =
            AssetManager::CreateNewAsset<Animation>(fp.stem().string() + ".hAnimation", fp.parent_path().string(), m_Animator->m_Animation);
        m_Animator->m_Animation->Handle = animationAsset->Handle;
        auto skeletonAsset =
            AssetManager::CreateNewAsset<Skeleton>(fp.stem().string() + ".hSkeleton", fp.parent_path().string(), m_Animator->m_Skeleton);
        m_Animator->m_Skeleton->Handle = skeletonAsset->Handle;

        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Animator";
        out << YAML::BeginMap;
        out << YAML::Key << "AssetHandle" << YAML::Value << m_Animator->Handle;
        out << YAML::Key << "Animation" << YAML::Value << animationAsset->Handle;
        out << YAML::Key << "Skeleton" << YAML::Value << skeletonAsset->Handle;
        out << YAML::EndMap;

        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }
    bool AnimatorSerializer::Deserialize(const std::string& filepath) {
        YNode data;
        try {
            data = YAML::LoadFile(filepath);
        } catch (YAML::ParserException& e) {
            ET_CORE_ERROR("Failed to load Skeleton file '{0}'\n     {1}", filepath, e.what());
            return false;
        }
        if (!data["Animator"]) return false;

        AssetHandle animationHandle = data["Animator"]["Animation"].as<AssetHandle>();
        AssetHandle skeletonHandle = data["Animator"]["Skeleton"].as<AssetHandle>();

        m_Animator->m_Animation = AssetManager::GetAsset<Animation>(animationHandle);
        m_Animator->m_Skeleton = AssetManager::GetAsset<Skeleton>(skeletonHandle);

        return true;
    }
}  // namespace Ethereal