#include "pch.h"
#include "AssetSerializer.h"
#include "AssetManager.h"
#include "Utils/StringUtils.h"
#include "Utils/FileSystem.h"
#include "Utils/YAMLSerializationHelpers.h"

#include "Renderer/Mesh.h"
#include "Renderer/MaterialAsset.h"
#include "Renderer/Environment.h"

#include "yaml-cpp/yaml.h"
#include <fstream>
#include <Core/GlobalContext.h>
#include <Scene/SceneSerializer.h>

#define ET_SERIALIZE_PROPERTY(propName, propVal, outputNode) outputNode << YAML::Key << #propName << YAML::Value << propVal

#define ET_SERIALIZE_PROPERTY_ASSET(propName, propVal, outputData) \
    outputData << YAML::Key << #propName << YAML::Value << (propVal ? (uint64_t)propVal->Handle : 0);

#define ET_DESERIALIZE_PROPERTY(propName, destination, node, defaultValue) \
    destination = node[#propName] ? node[#propName].as<decltype(defaultValue)>() : defaultValue

#define ET_DESERIALIZE_PROPERTY_ASSET(propName, destination, inputData, assetClass)               \
    {                                                                                             \
        AssetHandle assetHandle = inputData[#propName] ? inputData[#propName].as<uint64_t>() : 0; \
        if (AssetManager::IsAssetHandleValid(assetHandle)) {                                      \
            destination = AssetManager::GetAsset<assetClass>(assetHandle);                        \
        } else {                                                                                  \
            ET_CORE_ERROR("Tried to load invalid asset {0}.", #assetClass);                       \
        }                                                                                         \
    }

namespace Ethereal
{

    bool TextureSerializer::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const {
        asset = Texture2D::Create(AssetManager::GetFileSystemPathString(metadata));
        asset->Handle = metadata.Handle;

        bool result = asset.As<Texture2D>()->IsLoaded();

        if (!result) asset->SetFlag(AssetFlag::Invalid, true);

        return result;
    }

    bool MeshAssetSerializer::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const {
        asset = Ref<MeshSource>::Create(AssetManager::GetFileSystemPathString(metadata));
        asset->Handle = metadata.Handle;
        return (asset.As<MeshSource>())->GetVertices().size() > 0;  // Maybe?
    }

    void MaterialAssetSerializer::Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const {
        Ref<MaterialAsset> material = asset.As<MaterialAsset>();

        YAML::Emitter out;
        out << YAML::BeginMap;  // Material
        out << YAML::Key << "Material" << YAML::Value;
        {
            out << YAML::BeginMap;

            ET_SERIALIZE_PROPERTY(UseAlbedo, material->IsUseAlbedoMap(), out);
            ET_SERIALIZE_PROPERTY(UseNormal, material->IsUseNormalMap(), out);
            ET_SERIALIZE_PROPERTY(UseRoughness, material->IsUseRoughnessMap(), out);
            ET_SERIALIZE_PROPERTY(UseMetallic, material->IsUseMetallicMap(), out);
            ET_SERIALIZE_PROPERTY(UseOcclusion, material->IsUseOcclusionMap(), out);
            ET_SERIALIZE_PROPERTY(UseEmissive, material->IsUseEmissiveMap(), out);

            ET_SERIALIZE_PROPERTY(AlbedoColor, material->GetAlbedoColor(), out);
            ET_SERIALIZE_PROPERTY(Metallic, material->GetMetalness(), out);
            ET_SERIALIZE_PROPERTY(Roughness, material->GetRoughness(), out);
            ET_SERIALIZE_PROPERTY(Emission, material->GetEmission(), out);
            ET_SERIALIZE_PROPERTY(Transparency, material->GetTransparency(), out);

            {
                Ref<Texture2D> albedoMap = material->GetAlbedoMap();
                AssetHandle albedoMapHandle = albedoMap ? albedoMap->Handle : (AssetHandle)0;
                ET_SERIALIZE_PROPERTY(AlbedoMap, albedoMapHandle, out);
            }
            {
                Ref<Texture2D> normalMap = material->GetNormalMap();
                AssetHandle normalMapHandle = normalMap ? normalMap->Handle : (AssetHandle)0;
                ET_SERIALIZE_PROPERTY(NormalMap, normalMapHandle, out);
            }
            {
                Ref<Texture2D> metalnessMap = material->GetMetalnessMap();
                AssetHandle metalnessMapHandle = metalnessMap ? metalnessMap->Handle : (AssetHandle)0;
                ET_SERIALIZE_PROPERTY(MetalnessMap, metalnessMapHandle, out);
            }
            {
                Ref<Texture2D> roughnessMap = material->GetRoughnessMap();
                AssetHandle roughnessMapHandle = roughnessMap ? roughnessMap->Handle : (AssetHandle)0;
                ET_SERIALIZE_PROPERTY(RoughnessMap, roughnessMapHandle, out);
            }
            {
                Ref<Texture2D> occlusionMap = material->GetOcclusionMap();
                AssetHandle occlusionMapHandle = occlusionMap ? occlusionMap->Handle : (AssetHandle)0;
                ET_SERIALIZE_PROPERTY(occlusionMap, occlusionMapHandle, out);
            }
            {
                Ref<Texture2D> emissiveMap = material->GetEmissiveMap();
                AssetHandle emissiveMapHandle = emissiveMap ? emissiveMap->Handle : (AssetHandle)0;
                ET_SERIALIZE_PROPERTY(EmissiveMap, emissiveMapHandle, out);
            }

            out << YAML::EndMap;
        }
        out << YAML::EndMap;  // Material

        std::ofstream fout(AssetManager::GetFileSystemPath(metadata));
        fout << out.c_str();
    }

    bool MaterialAssetSerializer::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const {
        std::ifstream stream(AssetManager::GetFileSystemPath(metadata));
        if (!stream.is_open()) return false;

        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node root = YAML::Load(strStream.str());
        YAML::Node materialNode = root["Material"];

        bool transparent = false;
        ET_DESERIALIZE_PROPERTY(Transparent, transparent, materialNode, false);

        Ref<MaterialAsset> material = Ref<MaterialAsset>::Create(AssetManager::GetFileSystemPath(metadata).stem().string(), transparent);

        ET_DESERIALIZE_PROPERTY(AlbedoColor, material->GetAlbedoColor(), materialNode, glm::vec3(1.0f));
        ET_DESERIALIZE_PROPERTY(Metallic, material->GetMetalness(), materialNode, 0.0f);
        ET_DESERIALIZE_PROPERTY(Roughness, material->GetRoughness(), materialNode, 1.0f);
        ET_DESERIALIZE_PROPERTY(Emission, material->GetEmission(), materialNode, 0.0f);
        ET_DESERIALIZE_PROPERTY(Transparency, material->GetTransparency(), materialNode, 0.0f);

        AssetHandle albedoMap, normalMap, metalnessMap, roughnessMap, occlusionMap, emissiveMap;
        ET_DESERIALIZE_PROPERTY(AlbedoMap, albedoMap, materialNode, (AssetHandle)0);
        ET_DESERIALIZE_PROPERTY(NormalMap, normalMap, materialNode, (AssetHandle)0);
        ET_DESERIALIZE_PROPERTY(MetalnessMap, metalnessMap, materialNode, (AssetHandle)0);
        ET_DESERIALIZE_PROPERTY(RoughnessMap, roughnessMap, materialNode, (AssetHandle)0);
        ET_DESERIALIZE_PROPERTY(OcclusionMap, occlusionMap, materialNode, (AssetHandle)0);
        ET_DESERIALIZE_PROPERTY(EmissiveMap, emissiveMap, materialNode, (AssetHandle)0);

        if (albedoMap) {
            if (AssetManager::IsAssetHandleValid(albedoMap)) material->SetAlbedoMap(AssetManager::GetAsset<Texture2D>(albedoMap));
        }
        if (normalMap) {
            if (AssetManager::IsAssetHandleValid(normalMap)) material->SetNormalMap(AssetManager::GetAsset<Texture2D>(normalMap));
        }
        if (metalnessMap) {
            if (AssetManager::IsAssetHandleValid(metalnessMap)) material->SetMetalnessMap(AssetManager::GetAsset<Texture2D>(metalnessMap));
        }
        if (roughnessMap) {
            if (AssetManager::IsAssetHandleValid(roughnessMap)) material->SetRoughnessMap(AssetManager::GetAsset<Texture2D>(roughnessMap));
        }
        if (occlusionMap) {
            if (AssetManager::IsAssetHandleValid(occlusionMap)) material->SetOcclusionMap(AssetManager::GetAsset<Texture2D>(occlusionMap));
        }
        if (emissiveMap) {
            if (AssetManager::IsAssetHandleValid(emissiveMap)) material->SetEmissiveMap(AssetManager::GetAsset<Texture2D>(emissiveMap));
        }

        ET_DESERIALIZE_PROPERTY(UseAlbedo, material->IsUseAlbedoMap(), materialNode, false);
        ET_DESERIALIZE_PROPERTY(UseNormal, material->IsUseNormalMap(), materialNode, false);
        ET_DESERIALIZE_PROPERTY(UseRoughness, material->IsUseRoughnessMap(), materialNode, false);
        ET_DESERIALIZE_PROPERTY(UseMetallic, material->IsUseMetallicMap(), materialNode, false);
        ET_DESERIALIZE_PROPERTY(UseOcclusion, material->IsUseOcclusionMap(), materialNode, false);
        ET_DESERIALIZE_PROPERTY(UseEmissive, material->IsUseEmissiveMap(), materialNode, false);

        asset = material;
        asset->Handle = metadata.Handle;

        return true;
    }

    bool EnvironmentSerializer::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const {
        auto [radiance, irradiance] = GlobalContext::GetRenderSystem().CreateEnvironmentMap(AssetManager::GetFileSystemPathString(metadata));

        if (!radiance || !irradiance) return false;

        asset = Ref<Environment>::Create(radiance, irradiance);
        asset->Handle = metadata.Handle;

        return true;
    }

    void SceneAssetSerializer::Serialize(const AssetMetaData& metadata, const Ref<Asset>& asset) const {
        SceneSerializer serializer(asset.As<Scene>());
        serializer.Serialize(AssetManager::GetFileSystemPath(metadata).string());
    }

    bool SceneAssetSerializer::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) const {
        asset = Ref<Asset>::Create();
        asset->Handle = metadata.Handle;
        return true;
    }
}  // namespace Ethereal
