#include "AnimSerializer.h"
#include "Utils/YAMLSerializationHelpers.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace Ethereal
{
    void AnimSerializer::Serialize(const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;

        out << YAML::Key << "Animation";
        out << YAML::BeginMap;
        out << YAML::Key << "Name" << YAML::Value << m_Animation->m_Name;
        out << YAML::Key << "TotalFrames" << YAML::Value << m_Animation->m_TotalFrames;
        out << YAML::EndMap;

        out << YAML::Key << "KeyFrames" << YAML::Value << YAML::BeginSeq;

        for (auto& kf : m_Animation->m_KeyClips) {
            out << YAML::BeginMap;
            out << YAML::Key << "JointID" << YAML::Value << kf.JointID;
            out << YAML::Key << "States";
            for (auto& ks : kf.States) {
                out << YAML::BeginMap;
                out << YAML::Key << "Position" << YAML::Value << ks.Position;
                out << YAML::Key << "Rotation" << YAML::Value << ks.Rotation;
                out << YAML::Key << "Scale" << YAML::Value << ks.Scale;
                out << YAML::Key << "TimeStamp" << YAML::Value << ks.TimeStamp;
                out << YAML::EndMap;
            }
            out << YAML::EndMap;
        }

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool AnimSerializer::Deserialize(const std::string& filepath) {
        YAML::Node data;
        try {
            data = YAML::LoadFile(filepath);
        } catch (YAML::ParserException& e) {
            ET_CORE_ERROR("Failed to load Animation file '{0}'\n     {1}", filepath, e.what());
            return false;
        }
        if (!data["Animation"]) return false;

        m_Animation->m_Name = data["Animation"]["Name"].as<std::string>();
        m_Animation->m_TotalFrames = data["Animation"]["TotalFrames"].as<size_t>();

        auto keyFrames = data["KeyFrames"];
        if (keyFrames) {
            for (auto kf : keyFrames) {
                AnimKeyClip keyClip;
                keyClip.JointID = kf["JointID"].as<size_t>();
                auto keyStates = kf["States"];
                for (auto ks : keyStates) {
                    AnimState state{};
                    state.Position = ks["Position"].as<glm::vec3>();
                    state.Rotation = ks["Rotation"].as<glm::quat>();
                    state.Scale = ks["Scale"].as<glm::vec3>();
                    state.TimeStamp = ks["TimeStamp"].as<float>();
                    keyClip.States.push_back(state);
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
        out << YAML::EndMap;
        out << YAML::Key << "Joints" << YAML::Value << YAML::BeginSeq;

        SerializeJoint(m_Skeleton->m_Root, out);

        out << YAML::EndSeq;
        out << YAML::EndMap;

        std::ofstream fout(filepath);
        fout << out.c_str();
    }

    bool SkelSerializer::Deserialize(const std::string& filepath) {
        YAML::Node data;
        try {
            data = YAML::LoadFile(filepath);
        } catch (YAML::ParserException& e) {
            ET_CORE_ERROR("Failed to load Skeleton file '{0}'\n     {1}", filepath, e.what());
            return false;
        }
        if (!data["Skeleton"]) return false;

        m_Skeleton->m_Name = data["Skeleton"]["Name"].as<std::string>();

        auto joints = data["Joints"];
        if (joints) {
            m_Skeleton->m_Root = DeserializeJoint(joints);
        }
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

    Ref<Joint> SkelSerializer::DeserializeJoint(YAML::Node& node) {
        Ref<Joint> joint = Ref<Joint>::Create();
        joint->m_Name = node["m_Name"].as<std::string>();
        joint->m_ID = node["m_Name"].as<size_t>();
        joint->m_InitialPosition = node["m_Name"].as<glm::vec3>();
        joint->m_InitialRotation = node["m_Name"].as<glm::quat>();
        joint->m_InitialScale = node["m_Name"].as<glm::vec3>();
        joint->m_Children.clear();

        auto children = node["Children"];
        for (auto child : children) {
            joint->m_Children.emplace_back(DeserializeJoint(child));
        }
        return joint;
    }
}  // namespace Ethereal