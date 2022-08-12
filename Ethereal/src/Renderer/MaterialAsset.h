#pragma once
#include "Material.h"
#include "Asset/Asset.h"

#include <map>
namespace Ethereal
{
    class MaterialAsset : public Asset {
      public:
        MaterialAsset(const std::string& name = "Empty Name", bool transparent = false);
        MaterialAsset(Ref<Material> material);
        ~MaterialAsset();

        glm::vec3& GetAlbedoColor();
        void SetAlbedoColor(const glm::vec3& color);

        float& GetMetalness();
        void SetMetalness(float value);

        float& GetRoughness();
        void SetRoughness(float value);

        float& GetEmission();
        void SetEmission(float value);

        bool& IsUseAlbedoMap() { return m_Material->b_Albedo; };
        bool& IsUseMetallicMap() { return m_Material->b_Metallic; };
        bool& IsUseNormalMap() { return m_Material->b_Normal; };
        bool& IsUseOcclusionMap() { return m_Material->b_Occlusion; };
        bool& IsUseRoughnessMap() { return m_Material->b_Roughness; };
        bool& IsUseEmissiveMap() { return m_Material->b_Emissive; };

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

        Ref<Texture> GetEmissiveMap();
        void SetEmissiveMap(Ref<Texture> texture);
        void SetUseEmissiveMap(bool value);
        void ClearEmissiveMap();

        float& GetTransparency();
        void SetTransparency(float transparency);

        static AssetType GetStaticType() { return AssetType::Material; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

        Ref<Material> GetMaterial() const { return m_Material; }
        void SetMaterial(Ref<Material> material) { m_Material = material; }

        bool IsTransparent() const { return m_Transparent; }
        std::string GetName() const { return m_Material->GetName(); }

      private:
        void SetDefaults();

      private:
        Ref<Material> m_Material;
        bool m_Transparent = false;
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
