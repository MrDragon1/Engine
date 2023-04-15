#pragma once
#include <map>

#include "Core/Asset/Asset.h"
#include "Backend/Driver.h"
#include "Resource/Descriptor/Material.h"
namespace Ethereal {
enum class MaterialFlag { None = BIT(0), DepthTest = BIT(1), Blend = BIT(2), TwoSided = BIT(3), DisableShadowCasting = BIT(4) };

class MaterialAsset : public Asset {
   public:
    MaterialAsset(const std::string& name = "Empty Name", bool transparent = false);
    MaterialAsset(const Ref<MaterialAsset>& other);
    MaterialAsset(const MaterialDesc& desc) { Load(desc); };
    ~MaterialAsset();

    void Load(const MaterialDesc& desc);
    void Save(MaterialDesc& desc);

    Vector3& GetAlbedoColor();
    void SetAlbedoColor(const Vector3& color);

    float& GetMetalness();
    void SetMetalness(float value);

    float& GetRoughness();
    void SetRoughness(float value);

    float& GetEmission();
    void SetEmission(float value);

    bool& IsUseAlbedoMap() { return b_Albedo; };
    bool& IsUseMetallicMap() { return b_Metallic; };
    bool& IsUseNormalMap() { return b_Normal; };
    bool& IsUseOcclusionMap() { return b_Occlusion; };
    bool& IsUseRoughnessMap() { return b_Roughness; };

    // Textures
    Ref<Texture> GetAlbedoMap();
    void SetAlbedoMap(Ref<Texture> texture);
    void SetUseAlbedoMap(bool value);
    void ClearAlbedoMap();

    Ref<Texture> GetNormalMap();
    void SetNormalMap(Ref<Texture> texture);
    void SetUseNormalMap(bool value);
    void ClearNormalMap();

    Ref<Texture> GetMetalnessMap();
    void SetMetalnessMap(Ref<Texture> texture);
    void SetUseMetalnessMap(bool value);
    void ClearMetalnessMap();

    Ref<Texture> GetRoughnessMap();
    void SetRoughnessMap(Ref<Texture> texture);
    void SetUseRoughnessMap(bool value);
    void ClearRoughnessMap();

    Ref<Texture> GetOcclusionMap();
    void SetOcclusionMap(Ref<Texture> texture);
    void SetUseOcclusionMap(bool value);
    void ClearOcclusionMap();

    float& GetTransparency();
    void SetTransparency(float transparency);

    static AssetType GetStaticType() { return AssetType::Material; }
    virtual AssetType GetAssetType() const override { return GetStaticType(); }

    bool IsTransparent() const { return b_Transparent; }
    std::string GetName() const { return mName; }

   private:
    void SetDefaults();

   private:
    Ref<Texture> mAlbedoMap;
    Ref<Texture> mNormalMap;
    Ref<Texture> mMetallicMap;
    Ref<Texture> mRoughnessMap;
    Ref<Texture> mOcclusionMap;

    Vector3 mAlbedo = Vector3(1.0f);
    float mMetallic = 0.0f;
    float mRoughness = 1.0f;
    float mEmisstion = 0.0f;
    float mTransparency = 0.0f;

    // TODO : Fix this
    bool b_Albedo = false;
    bool b_Metallic = false;
    bool b_Roughness = false;
    bool b_Normal = false;
    bool b_Occlusion = false;

    bool b_Transparent = false;

    std::string mName;
};

class MaterialTable : public RefCounted {
   public:
    explicit MaterialTable(uint32_t materialCount = 1);
    explicit MaterialTable(Ref<MaterialTable> other);
    ~MaterialTable() = default;

    bool HasMaterial(uint32_t materialIndex) const {
        ET_CORE_ASSERT(materialIndex < GetMaterialCount(), "Material Index Out of Range!");
        return mMaterials[materialIndex] != nullptr;
    }
    void SetMaterial(uint32_t index, Ref<MaterialAsset> material);
    void ClearMaterial(uint32_t index);

    Ref<MaterialAsset> GetMaterial(uint32_t materialIndex) const {
        ET_CORE_ASSERT(HasMaterial(materialIndex));
        return mMaterials[materialIndex];
    }
    std::vector<Ref<MaterialAsset>>& GetMaterials() { return mMaterials; }
    const std::vector<Ref<MaterialAsset>>& GetMaterials() const { return mMaterials; }

    uint32_t GetMaterialCount() const { return mMaterials.size(); }
    void SetMaterialCount(uint32_t materialCount) { mMaterials.resize(materialCount); }

    void Clear();

   private:
    std::vector<Ref<MaterialAsset>> mMaterials;
};

}  // namespace Ethereal
