#pragma once
#include "Asset/Asset.h"

#include <map>
namespace Ethereal
{
    enum class MaterialFlag
    {
        None = BIT(0),
        DepthTest = BIT(1),
        Blend = BIT(2),
        TwoSided = BIT(3),
        DisableShadowCasting = BIT(4)
    };

    class MaterialAsset : public Asset {
      public:
        MaterialAsset(const std::string& name = "Empty Name", bool transparent = false);
        MaterialAsset(const Ref<MaterialAsset>& other);
        ~MaterialAsset();

        glm::vec3& GetAlbedoColor();
        void SetAlbedoColor(const glm::vec3& color);

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
        bool IsUsingNormalMap();
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

        bool IsTransparent() const { return m_Transparent; }
        std::string GetName() const { return m_Name; }

      private:
        void SetDefaults();

      private:
        Ref<Texture> m_AlbedoMap;
        Ref<Texture> m_NormalMap;
        Ref<Texture> m_MetallicMap;
        Ref<Texture> m_RoughnessMap;
        Ref<Texture> m_OcclusionMap;

        glm::vec3 m_Albedo = glm::vec3(1.0f);
        float m_Metallic = 0.0f;
        float m_Roughness = 1.0f;
        float m_Emisstion = 0.0f;
        float m_Transparency = 0.0f;

        // TODO : Fix this
        bool b_Albedo = false;
        bool b_Metallic = false;
        bool b_Roughness = false;
        bool b_Normal = false;
        bool b_Occlusion = false;

        bool m_Transparent = false;

        std::string m_Name;
    };

    class MaterialTable : public RefCounted {
      public:
        explicit MaterialTable(uint32_t materialCount = 1);
        explicit MaterialTable(Ref<MaterialTable> other);
        ~MaterialTable() = default;

        bool HasMaterial(uint32_t materialIndex) const { return m_Materials.find(materialIndex) != m_Materials.end(); }
        void SetMaterial(uint32_t index, Ref<MaterialAsset> material);
        void ClearMaterial(uint32_t index);

        Ref<MaterialAsset> GetMaterial(uint32_t materialIndex) const {
            ET_CORE_ASSERT(HasMaterial(materialIndex));
            return m_Materials.at(materialIndex);
        }
        std::map<uint32_t, Ref<MaterialAsset>>& GetMaterials() { return m_Materials; }
        const std::map<uint32_t, Ref<MaterialAsset>>& GetMaterials() const { return m_Materials; }

        uint32_t GetMaterialCount() const { return m_MaterialCount; }
        void SetMaterialCount(uint32_t materialCount) { m_MaterialCount = materialCount; }

        void Clear();

      private:
        std::map<uint32_t, Ref<MaterialAsset>> m_Materials;
        uint32_t m_MaterialCount;
    };

}  // namespace Ethereal
