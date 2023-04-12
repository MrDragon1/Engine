#include "MaterialAsset.h"

#include <Core/Asset/AssetManager.h>
#include <Ethereal/src/Base/GlobalContext.h>

#include "Core/Renderer/RenderResource.h"
namespace Ethereal {
MaterialAsset::MaterialAsset(const std::string &name, bool transparent) : b_Transparent(transparent) {
    m_Name = name;
    SetDefaults();
}
MaterialAsset::MaterialAsset(const Ref<MaterialAsset> &other) {
    m_Name = other->m_Name;

    m_AlbedoMap = other->m_AlbedoMap;
    m_NormalMap = other->m_NormalMap;
    m_MetallicMap = other->m_MetallicMap;
    m_RoughnessMap = other->m_RoughnessMap;
    m_OcclusionMap = other->m_OcclusionMap;

    m_Albedo = other->m_Albedo;
    m_Metallic = other->m_Metallic;
    m_Roughness = other->m_Roughness;
    m_Emisstion = other->m_Emisstion;
    m_Transparency = other->m_Transparency;
    b_Transparent = other->b_Transparent;

    b_Albedo = other->b_Albedo;
    b_Metallic = other->b_Metallic;
    b_Roughness = other->b_Roughness;
    b_Normal = other->b_Normal;
    b_Occlusion = other->b_Occlusion;
}

MaterialAsset::~MaterialAsset() {}

void MaterialAsset::Load(const MaterialDesc &desc) {
    Handle = desc.Handle;
    m_Name = desc.Name;

    m_AlbedoMap = AssetManager::GetAsset<Texture>(desc.AlbedoMap);
    m_NormalMap = AssetManager::GetAsset<Texture>(desc.NormalMap);
    m_MetallicMap = AssetManager::GetAsset<Texture>(desc.MetallicMap);
    m_RoughnessMap = AssetManager::GetAsset<Texture>(desc.RoughnessMap);
    m_OcclusionMap = AssetManager::GetAsset<Texture>(desc.OcclusionMap);

    m_Albedo = desc.Albedo;
    m_Metallic = desc.Metallic;
    m_Roughness = desc.Roughness;
    m_Emisstion = desc.Emisstion;
    m_Transparency = desc.Transparency;

    b_Albedo = desc.IsAlbedo;
    b_Metallic = desc.IsMetallic;
    b_Roughness = desc.IsRoughness;
    b_Normal = desc.IsNormal;
    b_Occlusion = desc.IsOcclusion;
    b_Transparent = desc.IsTransparent;
}

void MaterialAsset::Save(MaterialDesc &desc) {
    desc.Handle = Handle;

    desc.Name = m_Name;

    desc.AlbedoMap = m_AlbedoMap->Handle;
    desc.NormalMap = m_NormalMap->Handle;
    desc.MetallicMap = m_MetallicMap->Handle;
    desc.RoughnessMap = m_RoughnessMap->Handle;
    desc.OcclusionMap = m_OcclusionMap->Handle;

    desc.Albedo = m_Albedo;
    desc.Metallic = m_Metallic;
    desc.Roughness = m_Roughness;
    desc.Emisstion = m_Emisstion;
    desc.Transparency = m_Transparency;

    desc.IsAlbedo = b_Albedo;
    desc.IsMetallic = b_Metallic;
    desc.IsRoughness = b_Roughness;
    desc.IsNormal = b_Normal;
    desc.IsOcclusion = b_Occlusion;
    desc.IsTransparent = b_Transparent;
}

Vector3 &MaterialAsset::GetAlbedoColor() { return m_Albedo; }

void MaterialAsset::SetAlbedoColor(const Vector3 &color) { m_Albedo = color; }

float &MaterialAsset::GetMetalness() { return m_Metallic; }

void MaterialAsset::SetMetalness(float value) { m_Metallic = value; }

float &MaterialAsset::GetRoughness() { return m_Roughness; }

void MaterialAsset::SetRoughness(float value) { m_Roughness = value; }

float &MaterialAsset::GetEmission() { return m_Emisstion; }

void MaterialAsset::SetEmission(float value) { m_Emisstion = value; }

Ref<Texture> MaterialAsset::GetAlbedoMap() { return m_AlbedoMap; }

void MaterialAsset::SetAlbedoMap(Ref<Texture> texture) {
    m_AlbedoMap = texture;
    SetUseAlbedoMap(true);
}

void MaterialAsset::ClearAlbedoMap() {
    SetAlbedoMap(RenderResource::WhiteTexture);
    SetUseAlbedoMap(false);
}

Ref<Texture> MaterialAsset::GetNormalMap() { return m_NormalMap; }

void MaterialAsset::SetNormalMap(Ref<Texture> texture) {
    m_NormalMap = texture;
    SetUseNormalMap(true);
}

void MaterialAsset::SetUseNormalMap(bool value) { b_Normal = value; }

void MaterialAsset::ClearNormalMap() {
    SetNormalMap(RenderResource::WhiteTexture);
    SetUseNormalMap(false);
}
Ref<Texture> MaterialAsset::GetMetalnessMap() { return m_MetallicMap; }

void MaterialAsset::SetMetalnessMap(Ref<Texture> texture) {
    m_MetallicMap = texture;
    SetUseMetalnessMap(true);
}

void MaterialAsset::ClearMetalnessMap() {
    SetMetalnessMap(RenderResource::WhiteTexture);
    SetUseMetalnessMap(false);
}
Ref<Texture> MaterialAsset::GetRoughnessMap() { return m_RoughnessMap; }

void MaterialAsset::SetRoughnessMap(Ref<Texture> texture) {
    m_RoughnessMap = texture;
    SetUseRoughnessMap(true);
}

void MaterialAsset::ClearRoughnessMap() {
    SetRoughnessMap(RenderResource::WhiteTexture);
    SetUseRoughnessMap(false);
}
float &MaterialAsset::GetTransparency() { return m_Transparency; }

void MaterialAsset::SetTransparency(float transparency) { m_Transparency = transparency; }

void MaterialAsset::SetDefaults() {
    // Set defaults
    SetAlbedoColor(Vector3(0.8f));
    SetEmission(0.0f);
    SetMetalness(0.0f);
    SetRoughness(1.0f);

    // Maps
    SetAlbedoMap(RenderResource::WhiteTexture);
    SetNormalMap(RenderResource::WhiteTexture);
    SetMetalnessMap(RenderResource::WhiteTexture);
    SetRoughnessMap(RenderResource::WhiteTexture);
    SetOcclusionMap(RenderResource::WhiteTexture);

    SetUseNormalMap(false);
    SetUseMetalnessMap(false);
    SetUseRoughnessMap(false);
    SetUseOcclusionMap(false);
    SetUseAlbedoMap(false);
}
Ref<Texture> MaterialAsset::GetOcclusionMap() { return m_OcclusionMap; }

void MaterialAsset::SetOcclusionMap(Ref<Texture> texture) {
    m_OcclusionMap = texture;
    SetUseOcclusionMap(true);
}

void MaterialAsset::ClearOcclusionMap() {
    SetOcclusionMap(RenderResource::WhiteTexture);
    SetUseOcclusionMap(false);
}

void MaterialAsset::SetUseAlbedoMap(bool value) { b_Albedo = value; }

void MaterialAsset::SetUseMetalnessMap(bool value) { b_Metallic = value; }

void MaterialAsset::SetUseRoughnessMap(bool value) { b_Roughness = value; }

void MaterialAsset::SetUseOcclusionMap(bool value) { b_Occlusion = value; }

MaterialTable::MaterialTable(uint32_t materialCount) { SetMaterialCount(materialCount); }

MaterialTable::MaterialTable(Ref<MaterialTable> other) {
    SetMaterialCount(other->GetMaterialCount());
    const auto &meshMaterials = other->GetMaterials();
    for (int i = 0; i < GetMaterialCount(); i++) {
        SetMaterial(i, meshMaterials[i]);
    }
}

void MaterialTable::SetMaterial(uint32_t index, Ref<MaterialAsset> material) {
    // if (material->Handle == 0) ET_CORE_ERROR("Can not set material with handle 0");
    if (index >= GetMaterialCount()) m_Materials.resize(index + 1);
    m_Materials[index] = material;
}

void MaterialTable::ClearMaterial(uint32_t index) {
    ET_CORE_ASSERT(HasMaterial(index));
    m_Materials[index] = nullptr;
}

void MaterialTable::Clear() { m_Materials.clear(); }

}  // namespace Ethereal