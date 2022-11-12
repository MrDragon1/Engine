#include <Ethereal/src/Base/GlobalContext.h>
#include "MaterialAsset.h"

namespace Ethereal
{
    MaterialAsset::MaterialAsset(const std::string &name, bool transparent) : m_Transparent(transparent) {
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
        m_Transparent = other->m_Transparent;

        b_Albedo = other->b_Albedo;
        b_Metallic = other->b_Metallic;
        b_Roughness = other->b_Roughness;
        b_Normal = other->b_Normal;
        b_Occlusion = other->b_Occlusion;
    }

    MaterialAsset::~MaterialAsset() {}

    glm::vec3 &MaterialAsset::GetAlbedoColor() { return m_Albedo; }

    void MaterialAsset::SetAlbedoColor(const glm::vec3 &color) { m_Albedo = color; }

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
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetAlbedoMap(whiteTexture);
        SetUseAlbedoMap(false);
    }

    Ref<Texture> MaterialAsset::GetNormalMap() { return m_NormalMap; }

    void MaterialAsset::SetNormalMap(Ref<Texture> texture) {
        m_NormalMap = texture;
        SetUseNormalMap(true);
    }

    bool MaterialAsset::IsUsingNormalMap() { return b_Normal; }

    void MaterialAsset::SetUseNormalMap(bool value) { b_Normal = value; }

    void MaterialAsset::ClearNormalMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetNormalMap(whiteTexture);
        SetUseNormalMap(false);
    }
    Ref<Texture> MaterialAsset::GetMetalnessMap() { return m_MetallicMap; }

    void MaterialAsset::SetMetalnessMap(Ref<Texture> texture) {
        m_MetallicMap = texture;
        SetUseMetalnessMap(true);
    }

    void MaterialAsset::ClearMetalnessMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetMetalnessMap(whiteTexture);
        SetUseMetalnessMap(false);
    }
    Ref<Texture> MaterialAsset::GetRoughnessMap() { return m_RoughnessMap; }

    void MaterialAsset::SetRoughnessMap(Ref<Texture> texture) {
        m_RoughnessMap = texture;
        SetUseRoughnessMap(true);
    }

    void MaterialAsset::ClearRoughnessMap() {
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetRoughnessMap(whiteTexture);
        SetUseRoughnessMap(false);
    }
    float &MaterialAsset::GetTransparency() { return m_Transparency; }

    void MaterialAsset::SetTransparency(float transparency) { m_Transparency = transparency; }

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
        Ref<Texture> whiteTexture = GlobalContext::GetRenderSystem().GetWhiteTexture();
        SetOcclusionMap(whiteTexture);
        SetUseOcclusionMap(false);
    }

    void MaterialAsset::SetUseAlbedoMap(bool value) { b_Albedo = value; }

    void MaterialAsset::SetUseMetalnessMap(bool value) { b_Metallic = value; }

    void MaterialAsset::SetUseRoughnessMap(bool value) { b_Roughness = value; }

    void MaterialAsset::SetUseOcclusionMap(bool value) { b_Occlusion = value; }

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