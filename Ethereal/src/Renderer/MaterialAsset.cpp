#include "MaterialAsset.h"
#include "Utils/AssetManager.h"

namespace Ethereal
{
    MaterialAsset::MaterialAsset(bool transparent) : m_Transparent(transparent) {
        if (transparent) {
            // Use Transparent shader
            // m_Material = Material::Create();
        } else
            m_Material = Material::Create(Shader::Create("assets/shaders/PBR.glsl"));

        SetDefaults();
    }
    MaterialAsset::MaterialAsset(Ref<Material> material) { m_Material = Material::Copy(material); }

    MaterialAsset::~MaterialAsset() {}

    glm::vec3 &MaterialAsset::GetAlbedoColor() { return m_Material->m_Albedo; }

    void MaterialAsset::SetAlbedoColor(const glm::vec3 &color) { m_Material->SetAlbedo(color); }

    float &MaterialAsset::GetMetalness() { return m_Material->m_Metallic; }

    void MaterialAsset::SetMetalness(float value) { m_Material->SetMetallic(value); }

    float &MaterialAsset::GetRoughness() { return m_Material->m_Roughness; }

    void MaterialAsset::SetRoughness(float value) { m_Material->SetRoughness(value); }

    float &MaterialAsset::GetEmission() { return m_Material->m_Emisstion; }

    void MaterialAsset::SetEmission(float value) { m_Material->SetEmission(value); }

    Ref<Texture2D> MaterialAsset::GetAlbedoMap() { return m_Material->m_AlbedoMap; }

    void MaterialAsset::SetAlbedoMap(Ref<Texture2D> texture) { m_Material->SetAlbedoMap(texture); }

    void MaterialAsset::ClearAlbedoMap() {
        Ref<Texture2D> whiteTexture = TextureManager::AddTexture("assets/textures/default/default_diffuse.png");
        SetAlbedoMap(whiteTexture);
    }

    Ref<Texture2D> MaterialAsset::GetNormalMap() { return Ref<Texture2D>(); }

    void MaterialAsset::SetNormalMap(Ref<Texture2D> texture) { m_Material->SetNormalMap(texture); }

    bool MaterialAsset::IsUsingNormalMap() { return m_Material->b_Normal; }

    void MaterialAsset::SetUseNormalMap(bool value) { m_Material->SetUseNormalMap(value); }

    void MaterialAsset::ClearNormalMap() {
        Ref<Texture2D> whiteTexture = TextureManager::AddTexture("assets/textures/default/default_diffuse.png");
        SetNormalMap(whiteTexture);
    }
    Ref<Texture2D> MaterialAsset::GetMetalnessMap() { return Ref<Texture2D>(); }

    void MaterialAsset::SetMetalnessMap(Ref<Texture2D> texture) { m_Material->SetMetallicMap(texture); }

    void MaterialAsset::ClearMetalnessMap() {
        Ref<Texture2D> whiteTexture = TextureManager::AddTexture("assets/textures/default/default_diffuse.png");
        SetMetalnessMap(whiteTexture);
    }
    Ref<Texture2D> MaterialAsset::GetRoughnessMap() { return Ref<Texture2D>(); }

    void MaterialAsset::SetRoughnessMap(Ref<Texture2D> texture) { m_Material->SetRoughnessMap(texture); }

    void MaterialAsset::ClearRoughnessMap() {
        Ref<Texture2D> whiteTexture = TextureManager::AddTexture("assets/textures/default/default_diffuse.png");
        SetRoughnessMap(whiteTexture);
    }
    float &MaterialAsset::GetTransparency() { return m_Material->m_Transparency; }

    void MaterialAsset::SetTransparency(float transparency) { m_Material->SetTransparency(transparency); }

    void MaterialAsset::SetDefaults() {
        Ref<Texture2D> whiteTexture = TextureManager::AddTexture("assets/textures/default/default_diffuse.png");
        if (m_Transparent) {
            // Set defaults
            SetAlbedoColor(glm::vec3(0.8f));

            // Maps
            SetAlbedoMap(whiteTexture);
        } else {
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
        }
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