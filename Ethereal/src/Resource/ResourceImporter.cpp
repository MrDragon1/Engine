#include "ResourceImporter.h"

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
            if (m.second->IsValid() && !m.second->IsCreated()) {
                // TODO: create file in materials subdirectory, but this needs to create directory automatically which has not been implemented.
                MaterialDesc desc;
                m.second->Save(desc);
                AssetManager::CreateAsset_Ref(m.second->GetName(), path.parent_path().string(), desc);
            }
        }

        // Create animator that mesh used
        if (meshSource->IsAnimated()) {
            AnimatorDesc desc;
            meshSource->GetAnimator()->Save(desc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), desc);

            // meshSource->GetAnimator()->Handle = animatorAsset->Handle;
        }

        if (meshSource->IsAnimated()) {
            MeshDesc desc;
            Ref<Mesh> mesh = Ref<Mesh>::Create(meshSource, mt);
            mesh->Save(desc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), desc);
        } else {
            StaticMeshDesc desc;
            Ref<StaticMesh> mesh = Ref<StaticMesh>::Create(meshSource, mt);
            mesh->Save(desc);
            AssetManager::CreateAsset_Ref(metaData.FilePath.stem().string(), path.parent_path().string(), desc);
        }
    }

}  // namespace Ethereal
