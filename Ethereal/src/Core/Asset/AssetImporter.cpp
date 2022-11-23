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
            case AssetType::StaticMesh: {
                StaticMeshDesc desc;
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), desc);
                asset = Ref<StaticMesh>::Create(desc);
                break;
            }
            case AssetType::Mesh: {
                MeshDesc desc;
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), desc);
                asset = Ref<Mesh>::Create(desc);
                break;
            }
            case AssetType::Material: {
                MaterialDesc desc;
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), desc);
                asset = Ref<MaterialAsset>::Create(desc);
                break;
            }
            case AssetType::Animation: {
                AnimationDesc desc;
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), desc);
                asset = Ref<Animation>::Create(desc);
                break;
            }
            case AssetType::Skeleton: {
                SkeletonDesc desc;
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), desc);
                asset = Ref<Skeleton>::Create(desc);
                break;
            }
            case AssetType::Animator: {
                AnimatorDesc desc;
                AssetManager::LoadAsset_Ref(AssetManager::GetFileSystemPath(metadata).string(), desc);
                asset = Ref<Animator>::Create(desc);
                break;
            }
            default:
                return false;
        }
        asset->Handle = metadata.Handle;
        return true;
    }
}  // namespace Ethereal