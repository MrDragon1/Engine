#include <Core/GlobalContext.h>
#include "MaterialAsset.h"

namespace Ethereal
{
    MaterialAsset::MaterialAsset(const std::string &name, bool transparent) : m_Transparent(transparent) {
        if (transparent) {
            // Use Transparent shader
            // m_Material = Material::Create();
        } else
            m_Material = Material::Create(name);

        SetDefaults();
    }
    MaterialAsset::MaterialAsset(Ref<Material> material) {
        m_Material = Material::Create(material->GetName() + " Copy");
        SetAlbedoColor(material->m_Albedo);
        SetMetalness(material->m_Metallic);
        SetRoughness(material->m_Roughness);
        SetEmission(material->m_Emisstion);
        SetTransparency(material->m_Transparency);

        if (!material->m_AlbedoMap)
            ClearAlbedoMap();
        else
            SetAlbedoMap(material->m_AlbedoMap);
        if (!material->m_NormalMap)
            ClearNormalMap();
        else
            SetNormalMap(material->m_NormalMap);
        if (!material->m_MetallicMap)
            ClearMetalnessMap();
        else
            SetMetalnessMap(material->m_MetallicMap);
        if (!material->m_RoughnessMap)
            ClearRoughnessMap();
        else
            SetRoughnessMap(material->m_RoughnessMap);
        if (!material->m_OcclusionMap)
            ClearOcclusionMap();
        else
            SetOcclusionMap(material->m_OcclusionMap);
        if (!material->m_EmissiveMap)
            ClearEmissiveMap();
        else
            SetEmissiveMap(material->m_EmissiveMap);

        SetUseAlbedoMap(material->b_Albedo);
        SetUseNormalMap(material->b_Normal);
        SetUseMetalnessMap(material->b_Metallic);
        SetUseRoughnessMap(material->b_Roughness);
        SetUseOcclusionMap(material->b_Occlusion);
        SetUseEmissiveMap(material->b_Emissive);
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

    void MaterialAsset::SetAlbedoMap(Ref<Texture> texture) {
        m_Material->SetAlbedoMap(texture);
        SetUseAlbedoMap(true);
    }

    void MaterialAsset::ClearAlbedoMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetAlbedoMap(whiteTexture);
        SetUseAlbedoMap(false);
    }

    Ref<Texture> MaterialAsset::GetNormalMap() { return m_Material->m_NormalMap; }

    void MaterialAsset::SetNormalMap(Ref<Texture> texture) {
        m_Material->SetNormalMap(texture);
        SetUseNormalMap(true);
    }

    bool MaterialAsset::IsUsingNormalMap() { return m_Material->b_Normal; }

    void MaterialAsset::SetUseNormalMap(bool value) { m_Material->SetUseNormalMap(value); }

    void MaterialAsset::ClearNormalMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetNormalMap(whiteTexture);
        SetUseNormalMap(false);
    }
    Ref<Texture> MaterialAsset::GetMetalnessMap() { return m_Material->m_MetallicMap; }

    void MaterialAsset::SetMetalnessMap(Ref<Texture> texture) {
        m_Material->SetMetallicMap(texture);
        SetUseMetalnessMap(true);
    }

    void MaterialAsset::ClearMetalnessMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetMetalnessMap(whiteTexture);
        SetUseMetalnessMap(false);
    }
    Ref<Texture> MaterialAsset::GetRoughnessMap() { return m_Material->m_RoughnessMap; }

    void MaterialAsset::SetRoughnessMap(Ref<Texture> texture) {
        m_Material->SetRoughnessMap(texture);
        SetUseRoughnessMap(true);
    }

    void MaterialAsset::ClearRoughnessMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetRoughnessMap(whiteTexture);
        SetUseRoughnessMap(false);
    }
    float &MaterialAsset::GetTransparency() { return m_Material->m_Transparency; }

    void MaterialAsset::SetTransparency(float transparency) { m_Material->SetTransparency(transparency); }

    void MaterialAsset::SetDefaults() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        // Set defaults
        SetAlbedoColor(glm::vec3(0.8f));
        SetEmission(0.0f);
        SetMetalness(0.0f);
        SetRoughness(1.0f);

        // Maps
        SetAlbedoMap(whiteTexture);
        SetNormalMap(whiteTexture);
        SetMetalnessMap(whiteTexture);
        SetRoughnessMap(whiteTexture);
        SetOcclusionMap(whiteTexture);
        SetEmissiveMap(whiteTexture);

        SetUseNormalMap(false);
        SetUseMetalnessMap(false);
        SetUseRoughnessMap(false);
        SetUseOcclusionMap(false);
        SetUseEmissiveMap(false);
        SetUseAlbedoMap(false);
    }
    Ref<Texture> MaterialAsset::GetOcclusionMap() { return m_Material->m_OcclusionMap; }

    void MaterialAsset::SetOcclusionMap(Ref<Texture> texture) {
        m_Material->SetOcclusionMap(texture);
        SetUseOcclusionMap(true);
    }

    void MaterialAsset::ClearOcclusionMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetOcclusionMap(whiteTexture);
        SetUseOcclusionMap(false);
    }
    Ref<Texture> MaterialAsset::GetEmissiveMap() { return m_Material->m_EmissiveMap; }

    void MaterialAsset::SetEmissiveMap(Ref<Texture> texture) {
        m_Material->SetEmissiveMap(texture);
        SetUseEmissiveMap(true);
    }

    void MaterialAsset::ClearEmissiveMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetEmissiveMap(whiteTexture);
        SetUseEmissiveMap(false);
    }
    void MaterialAsset::SetUseAlbedoMap(bool value) { m_Material->SetUseAlbedoMap(value); }

    void MaterialAsset::SetUseMetalnessMap(bool value) { m_Material->SetUseMetallicMap(value); }

    void MaterialAsset::SetUseRoughnessMap(bool value) { m_Material->SetUseRoughnessMap(value); }

    void MaterialAsset::SetUseOcclusionMap(bool value) { m_Material->SetUseOcclusionMap(value); }

    void MaterialAsset::SetUseEmissiveMap(bool value) { m_Material->SetUseEmissiveMap(value); }

    MaterialTable::MaterialTable(uint32_t materialCount) : m_MaterialCount(materialCount) {}

    MaterialTable::MaterialTable(Ref<MaterialTable> other) : m_MaterialCount(other->m_MaterialCount) {
        const auto &meshMaterials = other->GetMaterials();
        for (auto [index, materialAsset] : meshMaterials) SetMaterial(index, materialAsset);
    }

    void MaterialTable::SetMaterial(uint32_t index, Ref<MaterialAsset> material) {
        if (material->Handle == 0) ET_CORE_ERROR("Can not set material with handle 0");
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