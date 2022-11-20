#include <Resource/ResourceLoader.h>
#include "pch.h"
#include "AssetImporter.h"
#include "AssetManager.h"
#include "Ethereal/src/Resource/Descriptor/Descriptor.h"

namespace Ethereal
{
    bool AssetImporter::TryLoadData(const AssetMetaData& metadata, Ref<Asset>& asset) {

        switch(metadata.Type){
            case AssetType::Texture:
                asset = ResourceLoader::LoadTexture(AssetManager::GetFileSystemPath(metadata).string());
                break;
            case AssetType::MeshSource:
                asset = ResourceLoader::LoadMeshSource(AssetManager::GetFileSystemPath(metadata).string());
                break;
            case AssetType::EnvMap:
                asset = ResourceLoader::LoadEnvironmentMap(AssetManager::GetFileSystemPath(metadata).string());
                break;
            case AssetType::StaticMesh:
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), *(asset.As<StaticMeshDesc>()));
                break;
            case AssetType::Mesh:
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), *(asset.As<MeshDesc>()));
                break;
            case AssetType::Material:{
                // TODO: refactor asset type like this way
                MaterialDesc desc;
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), desc);
                asset = Ref<MaterialDesc>::Create(desc);
                break;
            }
            case AssetType::Animation:
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), *(asset.As<AnimationDesc>()));
                break;
            case AssetType::Skeleton:
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), *(asset.As<SkeletonDesc>()));
                break;
            case AssetType::Animator:
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), *(asset.As<AnimatorDesc>()));
                break;
            default:
                return false;
        }
        asset->Handle = metadata.Handle;
        return true;
    }
}  // namespace Ethereal