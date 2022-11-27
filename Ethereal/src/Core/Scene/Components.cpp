#include "Components.h"

#include "Core/Asset/AssetManager.h"
#include "Core/Renderer/Mesh.h"

namespace Ethereal {

    void StaticMeshComponent::PostLoad() {
        materialTable = Ref<MaterialTable>::Create();
        auto mesh = AssetManager::GetAsset<StaticMesh>(StaticMeshHandle);
        if (mesh->GetMaterials()->GetMaterialCount() > MaterialTableRaw.Materials.size()) {
            materialTable->SetMaterialCount(mesh->GetMaterials()->GetMaterialCount());
        }

        // Get a material from meshComponent materialTable if it has (not the copy of the material)
        for (int index = 0; index < materialTable->GetMaterialCount(); index++) {
            if (mesh->GetMaterials()->HasMaterial(index)) {
                materialTable->SetMaterial(index, mesh->GetMaterials()->GetMaterial(index));
            }
        }

        // Override the materialTable
        for (int index = 0; index < materialTable->GetMaterialCount(); index++) {
            if(MaterialTableRaw.Materials.size() > index) { // TODO: remove this when support map reflection
                AssetHandle materialAsset = MaterialTableRaw.Materials[index];

                // User has specified an override material, just point to that material
                if (materialAsset && AssetManager::IsAssetHandleValid(materialAsset))
                    materialTable->SetMaterial(index, AssetManager::GetAsset<MaterialAsset>(materialAsset));
            }
        }
    }

    void MeshComponent::PostLoad() {
        materialTable = Ref<MaterialTable>::Create();
        auto mesh = AssetManager::GetAsset<Mesh>(MeshHandle);
        if (mesh->GetMaterials()->GetMaterialCount() > MaterialTableRaw.Materials.size()) {
            materialTable->SetMaterialCount(mesh->GetMaterials()->GetMaterialCount());
        }

        // Get a material from meshComponent materialTable if it has (not the copy of the material)
        for (int index = 0; index < materialTable->GetMaterialCount(); index++) {
            if (mesh->GetMaterials()->HasMaterial(index)) {
                materialTable->SetMaterial(index, mesh->GetMaterials()->GetMaterial(index));
            }
        }

        // Override the materialTable
        for (int index = 0; index < materialTable->GetMaterialCount(); index++) {
            AssetHandle materialAsset = MaterialTableRaw.Materials[index];

            // User has specified an override material, just point to that material
            if (materialAsset && AssetManager::IsAssetHandleValid(materialAsset))
                materialTable->SetMaterial(index, AssetManager::GetAsset<MaterialAsset>(materialAsset));
        }
    }
}
