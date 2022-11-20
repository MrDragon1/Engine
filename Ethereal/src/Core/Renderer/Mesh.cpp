#include "pch.h"
#include "Mesh.h"

#include "Base/GlobalContext.h"
#include "Core/Asset/AssetManager.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include "imgui.h"
#include <filesystem>

namespace Ethereal
{
    MeshSource::MeshSource() {}

    MeshSource::~MeshSource() {}

    void MeshSource::LoadMaterials(Ref<MaterialTable>& materials) {
        // Materials
        if (m_Scene->HasMaterials()) {
            ET_CORE_INFO("---- Materials - {0} ----", m_FilePath);
            materials->SetMaterialCount(m_Scene->mNumMaterials);
            for (uint32_t i = 0; i < m_Scene->mNumMaterials; i++) {
                auto aiMaterial = m_Scene->mMaterials[i];
                auto aiMaterialName = aiMaterial->GetName();
                ET_CORE_INFO("  {0} (Index = {1})", aiMaterialName.data, i);
                std::string materialName = aiMaterialName.C_Str();

                if (materialName == "DefaultMaterial") {
                    MaterialDesc desc;
                    AssetManager::LoadAsset_Ref("assets/materials/M_Default.EMaterial", desc);

                    auto mi = Ref<MaterialAsset>::Create();
                    mi->Load(desc);
                    materials->SetMaterial(i, mi);
                    continue;
                }

                auto mi = Ref<MaterialAsset>::Create(aiMaterialName.data);
                materials->SetMaterial(i, mi);

                aiString aiTexPath;
                Vector3 albedoColor(0.8f);
                float emission = 0.0f;
                aiColor3D aiColor, aiEmission;
                if (aiMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor) == AI_SUCCESS) albedoColor = {aiColor.r, aiColor.g, aiColor.b};

                if (aiMaterial->Get(AI_MATKEY_COLOR_EMISSIVE, aiEmission) == AI_SUCCESS) emission = aiEmission.r;

                mi->SetAlbedoColor(albedoColor);
                mi->SetEmission(emission);

                float shininess, metalness;
                if (aiMaterial->Get(AI_MATKEY_SHININESS, shininess) != aiReturn_SUCCESS) shininess = 80.0f;  // Default value

                if (aiMaterial->Get(AI_MATKEY_REFLECTIVITY, metalness) != aiReturn_SUCCESS) metalness = 0.0f;

                float roughness = 1.0f - sqrt(shininess / 100.0f);
                bool hasAlbedoMap = aiMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &aiTexPath) == AI_SUCCESS;
                mi->SetUseAlbedoMap(hasAlbedoMap);
                bool fallback = !hasAlbedoMap;
                if (hasAlbedoMap) {
                    // TODO: Temp - this should be handled by Filesystem
                    std::filesystem::path path = m_FilePath;
                    auto parentPath = path.parent_path();
                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();
                    auto texture = AssetManager::GetAsset<Texture>(texturePath);
                    if (texture && texture->IsLoaded()) {
                        mi->SetAlbedoMap(texture);
                        mi->SetAlbedoColor(Vector3(1.0f));
                    } else {
                        ET_CORE_ERROR("Could not load texture: {0}", texturePath);
                        fallback = true;
                    }
                }

                if (fallback) {
                    mi->ClearAlbedoMap();
                }

                // Normal maps
                bool hasNormalMap = aiMaterial->GetTexture(aiTextureType_NORMALS, 0, &aiTexPath) == AI_SUCCESS;
                mi->SetUseNormalMap(hasNormalMap);
                fallback = !hasNormalMap;
                if (hasNormalMap) {
                    // TODO: Temp - this should be handled by Filesystem
                    std::filesystem::path path = m_FilePath;
                    auto parentPath = path.parent_path();
                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();
                    auto texture = AssetManager::GetAsset<Texture>(texturePath);
                    if (texture && texture->IsLoaded()) {
                        mi->SetNormalMap(texture);
                    } else {
                        ET_CORE_ERROR("    Could not load texture: {0}", texturePath);
                        fallback = true;
                    }
                }

                if (fallback) {
                    mi->ClearNormalMap();
                    mi->SetUseNormalMap(false);
                }

                // Roughness map
                bool hasRoughnessMap = aiMaterial->GetTexture(aiTextureType_SHININESS, 0, &aiTexPath) == AI_SUCCESS;
                mi->SetUseRoughnessMap(hasRoughnessMap);
                fallback = !hasRoughnessMap;
                if (hasRoughnessMap) {
                    // TODO: Temp - this should be handled by Filesystem
                    std::filesystem::path path = m_FilePath;
                    auto parentPath = path.parent_path();
                    parentPath /= std::string(aiTexPath.data);
                    std::string texturePath = parentPath.string();
                    auto texture = AssetManager::GetAsset<Texture>(texturePath);
                    if (texture && texture->IsLoaded()) {
                        mi->SetRoughnessMap(texture);
                        mi->SetRoughness(1.0f);
                    } else {
                        ET_CORE_ERROR("    Could not load texture: {0}", texturePath);
                        fallback = true;
                    }
                }

                if (fallback) {
                    mi->ClearRoughnessMap();
                    mi->SetRoughness(roughness);
                }

                bool metalnessTextureFound = false;
                for (uint32_t p = 0; p < aiMaterial->mNumProperties; p++) {
                    auto prop = aiMaterial->mProperties[p];

                    if (prop->mType == aiPTI_String) {
                        uint32_t strLength = *(uint32_t*)prop->mData;
                        std::string str(prop->mData + 4, strLength);

                        std::string key = prop->mKey.data;
                        if (key == "$raw.ReflectionFactor|file") {
                            // TODO: Temp - this should be handled by Filesystem
                            std::filesystem::path path = m_FilePath;
                            auto parentPath = path.parent_path();
                            parentPath /= str;
                            std::string texturePath = parentPath.string();
                            auto texture = AssetManager::GetAsset<Texture>(texturePath);
                            if (texture && texture->IsLoaded()) {
                                metalnessTextureFound = true;
                                mi->SetMetalnessMap(texture);
                                mi->SetMetalness(1.0f);
                            } else {
                                ET_CORE_ERROR("    Could not load texture: {0}", texturePath);
                            }
                            break;
                        }
                    }
                }
                mi->SetUseMetalnessMap(metalnessTextureFound);
                fallback = !metalnessTextureFound;
                if (fallback) {
                    mi->ClearMetalnessMap();
                    mi->SetMetalness(metalness);
                }
            }
        } else {
            auto mi = Ref<MaterialAsset>::Create("Ethereal-Default");
            materials->SetMaterial(0, mi);
        }
    }

    StaticMesh::StaticMesh(Ref<MeshSource> meshSource, Ref<MaterialTable> materialTable) : m_MeshSource(meshSource) {
        // Generate a new asset handle
        Handle = {};

        SetSubmeshes({});
        m_Materials = Ref<MaterialTable>::Create();
        for (auto& m : materialTable->GetMaterials()) {
            m_Materials->SetMaterial(m.first, m.second);
        }
    }

    StaticMesh::StaticMesh(Ref<MeshSource> meshSource, const std::vector<uint32_t>& submeshes) : m_MeshSource(meshSource) {
        // Generate a new asset handle
        Handle = {};

        SetSubmeshes(submeshes);
        m_Materials = Ref<MaterialTable>::Create();
        //        const auto& meshMaterials = meshSource->GetMaterials();
        //        m_Materials = Ref<MaterialTable>::Create(meshMaterials.size());
        //        for (size_t i = 0; i < meshMaterials.size(); i++) m_Materials->SetMaterial(i, meshMaterials[i]);
    }

    StaticMesh::StaticMesh(const Ref<StaticMesh>& other) : m_MeshSource(other->m_MeshSource), m_Materials(other->m_Materials) {
        SetSubmeshes(other->m_Submeshes);
    }

    StaticMesh::~StaticMesh() {}

    void StaticMesh::Load(const StaticMeshDesc& desc) {
        m_MeshSource = AssetManager::GetAsset<MeshSource>(desc.Mesh);
        m_Materials = Ref<MaterialTable>::Create();
        for (int i = 0; i < desc.Materials.size(); i++) {
            auto materialdesc = AssetManager::GetAsset<MaterialDesc>(desc.Materials[i]);
            auto material = Ref<MaterialAsset>::Create();
            material->Load(*materialdesc);
            m_Materials->SetMaterial(i, material);
        }
        SetSubmeshes({});
    }

    void StaticMesh::Save(StaticMeshDesc& desc) {
        desc.Mesh = m_MeshSource->Handle;
        // TODO: Rewrite this when support map reflection
        int len = 0;
        for (auto& m : m_Materials->GetMaterials()) {
            len = len > m.first ? len : m.first;
        }
        desc.Materials.resize(len + 1);
        for (auto& m : m_Materials->GetMaterials()) {
            desc.Materials[m.first] = m.second->Handle;
        }
    }


    void StaticMesh::SetSubmeshes(const std::vector<uint32_t>& submeshes) {
        if (!submeshes.empty()) {
            m_Submeshes = submeshes;
        } else {
            const auto& submeshes = m_MeshSource->GetSubmeshes();
            m_Submeshes.resize(submeshes.size());
            for (uint32_t i = 0; i < submeshes.size(); i++) m_Submeshes[i] = i;
        }
    }

    Mesh::Mesh(Ref<MeshSource> meshSource, Ref<MaterialTable> materialTable) : m_MeshSource(meshSource) {
        // Generate a new asset handle
        Handle = {};

        SetSubmeshes({});
        m_Materials = Ref<MaterialTable>::Create();
        for (auto& m : materialTable->GetMaterials()) {
            m_Materials->SetMaterial(m.first, m.second);
        }
    }

    Mesh::Mesh(Ref<MeshSource> meshSource, const std::vector<uint32_t>& submeshes) : m_MeshSource(meshSource) {
        // Generate a new asset handle
        Handle = {};

        SetSubmeshes(submeshes);
        m_Materials = Ref<MaterialTable>::Create();
        //        const auto& meshMaterials = meshSource->GetMaterials();
        //        m_Materials = Ref<MaterialTable>::Create(meshMaterials.size());
        //        for (size_t i = 0; i < meshMaterials.size(); i++) m_Materials->SetMaterial(i, meshMaterials[i]);
    }

    Mesh::Mesh(const Ref<Mesh>& other) : m_MeshSource(other->m_MeshSource), m_Materials(other->m_Materials) { SetSubmeshes(other->m_Submeshes); }

    Mesh::~Mesh() {}

    void Mesh::Load(const MeshDesc& desc) {
        m_MeshSource = AssetManager::GetAsset<MeshSource>(desc.Mesh);
        m_Materials = Ref<MaterialTable>::Create();
        for (int i = 0; i < desc.Materials.size(); i++) {
            auto material = AssetManager::GetAsset<MaterialAsset>(desc.Materials[i]);
            m_Materials->SetMaterial(i, material);
        }
        SetSubmeshes({});

        // TODO: load animator according desc, and set animator to mesh
        // but for now, animator info is loaded in mesh source
    }

    void Mesh::Save(MeshDesc& desc) {
        desc.Mesh = m_MeshSource->Handle;
        // TODO: Rewrite this when support map reflection
        int len = 0;
        for (auto& m : m_Materials->GetMaterials()) {
            len = len > m.first ? len : m.first;
        }
        desc.Materials.resize(len + 1);
        for (auto& m : m_Materials->GetMaterials()) {
            desc.Materials[m.first] = m.second->Handle;
        }

        desc.Animator = m_MeshSource->GetAnimator()->Handle;
    }


    void Mesh::SetSubmeshes(const std::vector<uint32_t>& submeshes) {
        if (!submeshes.empty()) {
            m_Submeshes = submeshes;
        } else {
            const auto& submeshes = m_MeshSource->GetSubmeshes();
            m_Submeshes.resize(submeshes.size());
            for (uint32_t i = 0; i < submeshes.size(); i++) m_Submeshes[i] = i;
        }
    }
}  // namespace Ethereal