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
        } catch (YAML::ParserException e) {
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
                    AnimState state;
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
}  // namespace Ethereal