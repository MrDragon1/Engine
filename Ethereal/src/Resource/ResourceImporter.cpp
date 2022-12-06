#include "ResourceImporter.h"
#include "ResourceLoader.h"
namespace Ethereal
{
    bool ResourceImporter::Import(const std::string& filepath) {
        AssetHandle handle = AssetManager::LoadAsset(filepath);
        AssetMetaData metaData = AssetManager::GetMetadata(handle);
        if (!metaData.IsValid()) {
            ET_CORE_ASSERT(false, "Failed to load asset");
            return false;
        }
        if (metaData.Type == AssetType::MeshSource) return ImportModel(handle, metaData);
    }

    bool ResourceImporter::ImportModel(AssetHandle& handle, AssetMetaData& metaData) {
        auto meshSource = AssetManager::GetAsset<MeshSource>(metaData.Handle);
        if (!meshSource->IsValid()) return false;
        std::filesystem::path path = AssetManager::GetFileSystemPath(metaData);

        // Create materials that mesh used
        Ref<MaterialTable> mt = Ref<MaterialTable>::Create();
        meshSource->LoadMaterials(mt);
        for (auto& m : mt->GetMaterials()) {
            if (m->IsValid() && !m->IsCreated()) {
                // TODO: create file in materials subdirectory, but this needs to create directory automatically which has not been implemented.
                MaterialDesc desc;
                m->Save(desc);
                AssetManager::CreateAsset_Ref(m->GetName(), path.parent_path().string(), desc);
                m->Load(desc);
            }
        }

        // Create animator that mesh used
        if (meshSource->IsAnimated()) {
            AnimationDesc animationDesc;
            meshSource->GetAnimator()->m_Animation->Save(animationDesc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), animationDesc);
            meshSource->GetAnimator()->m_Animation->Load(animationDesc);

            SkeletonDesc skeletonDesc;
            meshSource->GetAnimator()->m_Skeleton->Save(skeletonDesc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), skeletonDesc);
            meshSource->GetAnimator()->m_Skeleton->Load(skeletonDesc);

            AnimatorDesc desc;
            meshSource->GetAnimator()->Save(desc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), desc);
            meshSource->GetAnimator()->Load(desc);
        }

        if (meshSource->IsAnimated()) {
            MeshDesc desc;
            Ref<Mesh> mesh = Ref<Mesh>::Create(meshSource, mt);
            mesh->Save(desc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), desc);
            mesh->Load(desc);
        } else {
            StaticMeshDesc desc;
            Ref<StaticMesh> mesh = Ref<StaticMesh>::Create(meshSource, mt);
            mesh->Save(desc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), desc);
            mesh->Load(desc);
        }
    }

}  // namespace Ethereal
