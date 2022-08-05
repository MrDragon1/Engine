#include <Core/GlobalContext.h>
#include "MaterialAsset.h"
#include "Utils/AssetManager.h"

namespace Ethereal
{
    MaterialAsset::MaterialAsset(bool transparent) : m_Transparent(transparent) {
        if (transparent) {
            // Use Transparent shader
            // m_Material = Material::Create();
        } else
            m_Material = Material::Create();

        SetDefaults();
    }
    MaterialAsset::MaterialAsset(Ref<Material> material) {
        m_Material = Material::Copy(material);
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        if (!m_Material->m_AlbedoMap) SetAlbedoMap(whiteTexture);
        if (!m_Material->m_NormalMap) SetNormalMap(whiteTexture);
        if (!m_Material->m_MetallicMap) SetMetalnessMap(whiteTexture);
        if (!m_Material->m_RoughnessMap) SetRoughnessMap(whiteTexture);
        if (!m_Material->m_OcclusionMap) SetOcclusionMap(whiteTexture);
        if (!m_Material->m_EmissiveMap) SetEmissiveMap(whiteTexture);
    }

    MaterialAsset::~MaterialAsset() {}

    glm::vec3 &MaterialAsset::GetAlbedoColor() { return m_Material->m_Albedo; }

    void MaterialAsset::SetAlbedoColor(const glm::vec3 &color) { m_Material->SetAlbedo(color); }

    float &MaterialAsset::GetMetalness() { return m_Material->m_Metallic; }

    void MaterialAsset::SetMetalness(float value) { m_Material->SetMetallic(value); }

    float &MaterialAsset::GetRoughness() { return m_Material->m_Roughness; }

    void MaterialAsset::SetRoughness(float value) { m_Material->SetRoughness(value); }

    float &MaterialAsset::GetEmission() { return m_Material->m_Emisstion; }

    void MaterialAsset::SetEmission(float value) { m_Material->SetEmission(value); }

    Ref<Texture> MaterialAsset::GetAlbedoMap() { return m_Material->m_AlbedoMap; }

    void MaterialAsset::SetAlbedoMap(Ref<Texture> texture) { m_Material->SetAlbedoMap(texture); }

    void MaterialAsset::ClearAlbedoMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetAlbedoMap(whiteTexture);
    }

    Ref<Texture> MaterialAsset::GetNormalMap() { return m_Material->m_NormalMap; }

    void MaterialAsset::SetNormalMap(Ref<Texture> texture) { m_Material->SetNormalMap(texture); }

    bool MaterialAsset::IsUsingNormalMap() { return m_Material->b_Normal; }

    void MaterialAsset::SetUseNormalMap(bool value) { m_Material->SetUseNormalMap(value); }

    void MaterialAsset::ClearNormalMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetNormalMap(whiteTexture);
    }
    Ref<Texture> MaterialAsset::GetMetalnessMap() { return m_Material->m_MetallicMap; }

    void MaterialAsset::SetMetalnessMap(Ref<Texture> texture) { m_Material->SetMetallicMap(texture); }

    void MaterialAsset::ClearMetalnessMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetMetalnessMap(whiteTexture);
    }
    Ref<Texture> MaterialAsset::GetRoughnessMap() { return m_Material->m_RoughnessMap; }

    void MaterialAsset::SetRoughnessMap(Ref<Texture> texture) { m_Material->SetRoughnessMap(texture); }

    void MaterialAsset::ClearRoughnessMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetRoughnessMap(whiteTexture);
    }
    float &MaterialAsset::GetTransparency() { return m_Material->m_Transparency; }

    void MaterialAsset::SetTransparency(float transparency) { m_Material->SetTransparency(transparency); }

    void MaterialAsset::SetDefaults() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        // Set defaults
        SetAlbedoColor(glm::vec3(0.8f));
        SetEmission(0.0f);
        SetUseNormalMap(false);
        SetMetalness(0.0f);
        SetRoughness(0.4f);

        // Maps
        SetAlbedoMap(whiteTexture);
        SetNormalMap(whiteTexture);
        SetMetalnessMap(whiteTexture);
        SetRoughnessMap(whiteTexture);
        SetOcclusionMap(whiteTexture);
        SetEmissiveMap(whiteTexture);
    }
    Ref<Texture> MaterialAsset::GetOcclusionMap() { return m_Material->m_OcclusionMap; }

    void MaterialAsset::SetOcclusionMap(Ref<Texture> texture) { m_Material->SetOcclusionMap(texture); }

    void MaterialAsset::ClearOcclusionMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetOcclusionMap(whiteTexture);
    }
    Ref<Texture> MaterialAsset::GetEmissiveMap() { return m_Material->m_EmissiveMap; }

    void MaterialAsset::SetEmissiveMap(Ref<Texture> texture) { m_Material->SetEmissiveMap(texture); }

    void MaterialAsset::ClearEmissiveMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetEmissiveMap(whiteTexture);
    }

    MaterialTable::MaterialTable(uint32_t materialCount) : m_MaterialCount(materialCount) {}

    MaterialTable::MaterialTable(Ref<MaterialTable> other) : m_MaterialCount(other->m_MaterialCount) {
        const auto &meshMaterials = other->GetMaterials();
        for (auto [index, materialAsset] : meshMaterials)
            // SetMaterial(index, Ref<MaterialAsset>::Create(materialAsset->GetMaterial()));
            SetMaterial(index, materialAsset);
    }

    void MaterialTable::SetMaterial(uint32_t index, Ref<MaterialAsset> material) {
        m_Materials[index] = material;
        if (index >= m_MaterialCount) m_MaterialCount = index + 1;
    }

    void MaterialTable::ClearMaterial(uint32_t index) {
        ET_CORE_ASSERT(HasMaterial(index));
        m_Materials.erase(index);
        if (index >= m_MaterialCount) m_MaterialCount = index + 1;
    }

    void MaterialTable::Clear() { m_Materials.clear(); }

}  // namespace Ethereal