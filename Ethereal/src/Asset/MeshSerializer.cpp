#include "pch.h"
#include "MeshSerializer.h"
#include "AssetManager.h"
#include "Utils/YAMLSerializationHelpers.h"

#include "yaml-cpp/yaml.h"
#include <fstream>

namespace YAML
{

    template <>
    struct convert<std::vector<uint32_t>> {
        static Node encode(const std::vector<uint32_t>& value) {
            Node node;
            for (uint32_t element : value) node.push_back(element);
            return node;
        }

        static bool decode(const Node& node, std::vector<uint32_t>& result) {
            if (!node.IsSequence()) return false;

            result.resize(node.size());
            for (size_t i = 0; i < node.size(); i++) result[i] = node[i].as<uint32_t>();

            return true;
        }
    };

}  // namespace YAML

namespace Ethereal
{

    YAML::Emitter& operator<<(YAML::Emitter& out, const std::vector<uint32_t>& value) {
        out << YAML::Flow;
        out << YAML::BeginSeq;
        for (uint32_t element : value) out << element;
        out << YAML::EndSeq;
        return out;
    }

    StaticMeshSerializer::StaticMeshSerializer() {}

    bool StaticMeshSerializer::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const {
        // TODO: this needs to open up a Hazel Mesh file and make sure
        //       the MeshAsset file is also loaded
        auto filepath = Project::GetAssetDirectory() / metadata.FilePath;
        std::ifstream stream(filepath);
        ET_CORE_ASSERT(stream);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Mesh"]) return false;

        YAML::Node rootNode = data["Mesh"];
        if (!rootNode["MeshAsset"] && !rootNode["MeshSource"]) return false;

        AssetHandle meshSourceHandle = rootNode["MeshSource"].as<uint64_t>();
        Ref<MeshSource> meshSource = AssetManager::GetAsset<MeshSource>(meshSourceHandle);
        if (!meshSource) return false;

        auto submeshIndices = rootNode["SubmeshIndices"].as<std::vector<uint32_t>>();
        Ref<StaticMesh> mesh = Ref<StaticMesh>::Create(meshSource, submeshIndices);

        if (rootNode["MaterialTable"]) {
            // The line below is for temporary, StaticMesh Create should not load material information.
            // The material info should be created when imported the model files which will create the .hsmesh file.
            mesh->GetMaterials()->Clear();

            YAML::Node materialTableNode = rootNode["MaterialTable"];
            for (auto materialEntry : materialTableNode) {
                auto index = materialEntry.first.as<uint32_t>();
                auto materialAsset = materialEntry.second.as<AssetHandle>();
                mesh->GetMaterials()->SetMaterial(index, AssetManager::GetAsset<Material>(materialAsset));
            }
        } else
            ET_CORE_WARN("No material table found for mesh {0}", metadata.FilePath.stem().string());

        mesh->Handle = metadata.Handle;
        asset = mesh;
        return true;
    }

    void StaticMeshSerializer::Serialize(Ref<StaticMesh> mesh, const std::string& filepath) {
        YAML::Emitter out;
        out << YAML::BeginMap;
        out << YAML::Key << "Mesh";
        {
            out << YAML::BeginMap;
            out << YAML::Key << "MeshSource" << YAML::Value << mesh->GetMeshSource()->Handle;

            out << YAML::Key << "MaterialTable" << YAML::Value << YAML::BeginMap;
            for (uint32_t i = 0; i < mesh->GetMaterials()->GetMaterialCount(); i++) {
                out << YAML::Key << i << YAML::Value << mesh->GetMaterials()->GetMaterial(i)->Handle;
            }
            out << YAML::EndMap;

            out << YAML::Key << "SubmeshIndices";
            out << YAML::Flow;
            if (mesh->GetSubmeshes().size() == mesh->GetMeshSource()->GetSubmeshes().size())
                out << YAML::Value << std::vector<uint32_t>();
            else
                out << YAML::Value << mesh->GetSubmeshes();

            out << YAML::EndMap;
        }
        out << YAML::EndMap;

        auto serializePath = Project::GetActive()->GetAssetDirectory() / filepath;
        ET_CORE_WARN("Serializing to {0}", serializePath.string());
        std::ofstream fout(serializePath);
        ET_CORE_ASSERT(fout.good());
        if (fout.good()) fout << out.c_str();
    }

    void StaticMeshSerializer::Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const {
        StaticMeshSerializer serializer;
        serializer.Serialize(asset.As<StaticMesh>(), metadata.FilePath.string());
    }

    void StaticMeshSerializer::SerializeRuntime(Ref<StaticMesh> mesh, const std::string& filepath) { ET_CORE_ASSERT(false); }

    bool StaticMeshSerializer::Deserialize(const std::string& filepath) { return false; }

    bool StaticMeshSerializer::DeserializeRuntime(const std::string& filepath) {
        ET_CORE_ASSERT(false);
        return false;
    }

}  // namespace Ethereal
