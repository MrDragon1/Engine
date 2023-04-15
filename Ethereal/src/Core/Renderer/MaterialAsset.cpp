#include "MaterialAsset.h"

#include <Core/Asset/AssetManager.h>
#include <Ethereal/src/Base/GlobalContext.h>

#include "Core/Renderer/RenderResource.h"
namespace Ethereal {
MaterialAsset::MaterialAsset(const std::string &name, bool transparent) : b_Transparent(transparent) {
    mName = name;
    SetDefaults();
}
MaterialAsset::MaterialAsset(const Ref<MaterialAsset> &other) {
    mName = other->mName;

    mAlbedoMap = other->mAlbedoMap;
    mNormalMap = other->mNormalMap;
    mMetallicMap = other->mMetallicMap;
    mRoughnessMap = other->mRoughnessMap;
    mOcclusionMap = other->mOcclusionMap;

    mAlbedo = other->mAlbedo;
    mMetallic = other->mMetallic;
    mRoughness = other->mRoughness;
    mEmisstion = other->mEmisstion;
    mTransparency = other->mTransparency;
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
    mName = desc.Name;

    mAlbedoMap = AssetManager::GetAsset<Texture>(desc.AlbedoMap);
    mNormalMap = AssetManager::GetAsset<Texture>(desc.NormalMap);
    mMetallicMap = AssetManager::GetAsset<Texture>(desc.MetallicMap);
    mRoughnessMap = AssetManager::GetAsset<Texture>(desc.RoughnessMap);
    mOcclusionMap = AssetManager::GetAsset<Texture>(desc.OcclusionMap);

    mAlbedo = desc.Albedo;
    mMetallic = desc.Metallic;
    mRoughness = desc.Roughness;
    mEmisstion = desc.Emisstion;
    mTransparency = desc.Transparency;

    b_Albedo = desc.IsAlbedo;
    b_Metallic = desc.IsMetallic;
    b_Roughness = desc.IsRoughness;
    b_Normal = desc.IsNormal;
    b_Occlusion = desc.IsOcclusion;
    b_Transparent = desc.IsTransparent;
}

void MaterialAsset::Save(MaterialDesc &desc) {
    desc.Handle = Handle;

    desc.Name = mName;

    desc.AlbedoMap = mAlbedoMap->Handle;
    desc.NormalMap = mNormalMap->Handle;
    desc.MetallicMap = mMetallicMap->Handle;
    desc.RoughnessMap = mRoughnessMap->Handle;
    desc.OcclusionMap = mOcclusionMap->Handle;

    desc.Albedo = mAlbedo;
    desc.Metallic = mMetallic;
    desc.Roughness = mRoughness;
    desc.Emisstion = mEmisstion;
    desc.Transparency = mTransparency;

    desc.IsAlbedo = b_Albedo;
    desc.IsMetallic = b_Metallic;
    desc.IsRoughness = b_Roughness;
    desc.IsNormal = b_Normal;
    desc.IsOcclusion = b_Occlusion;
    desc.IsTransparent = b_Transparent;
}

Vector3 &MaterialAsset::GetAlbedoColor() { return mAlbedo; }

void MaterialAsset::SetAlbedoColor(const Vector3 &color) { mAlbedo = color; }

float &MaterialAsset::GetMetalness() { return mMetallic; }

void MaterialAsset::SetMetalness(float value) { mMetallic = value; }

float &MaterialAsset::GetRoughness() { return mRoughness; }

void MaterialAsset::SetRoughness(float value) { mRoughness = value; }

float &MaterialAsset::GetEmission() { return mEmisstion; }

void MaterialAsset::SetEmission(float value) { mEmisstion = value; }

Ref<Texture> MaterialAsset::GetAlbedoMap() { return mAlbedoMap; }

void MaterialAsset::SetAlbedoMap(Ref<Texture> texture) {
    mAlbedoMap = texture;
    SetUseAlbedoMap(true);
}

void MaterialAsset::ClearAlbedoMap() {
    SetAlbedoMap(RenderResource::WhiteTexture);
    SetUseAlbedoMap(false);
}

Ref<Texture> MaterialAsset::GetNormalMap() { return mNormalMap; }

void MaterialAsset::SetNormalMap(Ref<Texture> texture) {
    mNormalMap = texture;
    SetUseNormalMap(true);
}

void MaterialAsset::SetUseNormalMap(bool value) { b_Normal = value; }

void MaterialAsset::ClearNormalMap() {
    SetNormalMap(RenderResource::WhiteTexture);
    SetUseNormalMap(false);
}
Ref<Texture> MaterialAsset::GetMetalnessMap() { return mMetallicMap; }

void MaterialAsset::SetMetalnessMap(Ref<Texture> texture) {
    mMetallicMap = texture;
    SetUseMetalnessMap(true);
}

void MaterialAsset::ClearMetalnessMap() {
    SetMetalnessMap(RenderResource::WhiteTexture);
    SetUseMetalnessMap(false);
}
Ref<Texture> MaterialAsset::GetRoughnessMap() { return mRoughnessMap; }

void MaterialAsset::SetRoughnessMap(Ref<Texture> texture) {
    mRoughnessMap = texture;
    SetUseRoughnessMap(true);
}

void MaterialAsset::ClearRoughnessMap() {
    SetRoughnessMap(RenderResource::WhiteTexture);
    SetUseRoughnessMap(false);
}
float &MaterialAsset::GetTransparency() { return mTransparency; }

void MaterialAsset::SetTransparency(float transparency) { mTransparency = transparency; }

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
Ref<Texture> MaterialAsset::GetOcclusionMap() { return mOcclusionMap; }

void MaterialAsset::SetOcclusionMap(Ref<Texture> texture) {
    mOcclusionMap = texture;
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
    if (index >= GetMaterialCount()) mMaterials.resize(index + 1);
    mMaterials[index] = material;
}

void MaterialTable::ClearMaterial(uint32_t index) {
    ET_CORE_ASSERT(HasMaterial(index));
    mMaterials[index] = nullptr;
}

void MaterialTable::Clear() { mMaterials.clear(); }

}  // namespace Ethereal