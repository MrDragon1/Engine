#pragma once
#include "AssetSerializer.h"
#include "Renderer/Mesh.h"

namespace Ethereal
{
    class StaticMeshSerializer : public AssetSerializer {
      public:
        StaticMeshSerializer();

        void Serialize(Ref<StaticMesh> mesh, const std::string& filepath);
        void SerializeRuntime(Ref<StaticMesh> mesh, const std::string& filepath);

        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);

        virtual void Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const override;
        virtual bool TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const override;
    };

    class MeshSerializer : public AssetSerializer {
      public:
        MeshSerializer();

        void Serialize(Ref<Mesh> mesh, const std::string& filepath);
        void SerializeRuntime(Ref<Mesh> mesh, const std::string& filepath);

        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);

        virtual void Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const override;
        virtual bool TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const override;
    };

}  // namespace Ethereal
