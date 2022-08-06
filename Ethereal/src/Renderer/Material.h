#pragma once
#include "Texture.h"
#include "Shader.h"
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

    class Material : public RefCounted {
      public:
        static Ref<Material> Create(const std::string& name = "");
        static Ref<Material> Copy(const Ref<Material>& other, const std::string& name = "");
        virtual ~Material() {}

        void SetAlbedo(const glm::vec3& value) { m_Albedo = value; }
        void SetMetallic(float value) { m_Metallic = value; }
        void SetRoughness(float value) { m_Roughness = value; }
        void SetEmission(float value) { m_Emisstion = value; }
        void SetTransparency(float value) { m_Transparency = value; }

        void SetAlbedoMap(Ref<Texture> value) { m_AlbedoMap = value; }
        void SetNormalMap(Ref<Texture> value) { m_NormalMap = value; }
        void SetMetallicMap(Ref<Texture> value) { m_MetallicMap = value; }
        void SetRoughnessMap(Ref<Texture> value) { m_RoughnessMap = value; }
        void SetOcclusionMap(Ref<Texture> value) { m_OcclusionMap = value; }
        void SetEmissiveMap(Ref<Texture> value) { m_EmissiveMap = value; }

        void SetUseNormalMap(bool value) { b_Normal = value; }
        void SetUseAlbedoMap(bool value) { b_Albedo = value; }
        void SetUseMetallicMap(bool value) { b_Metallic = value; }
        void SetUseRoughnessMap(bool value) { b_Roughness = value; }
        void SetUseOcclusionMap(bool value) { b_Occlusion = value; }
        void SetUseEmissiveMap(bool value) { b_Emissive = value; }

        virtual uint32_t GetFlags() const = 0;
        virtual void SetFlags(uint32_t flags) = 0;

        virtual bool GetFlag(MaterialFlag flag) const = 0;
        virtual void SetFlag(MaterialFlag flag, bool value = true) = 0;

        virtual const std::string& GetName() const = 0;

      private:
        Ref<Texture> m_AlbedoMap;
        Ref<Texture> m_NormalMap;
        Ref<Texture> m_MetallicMap;
        Ref<Texture> m_RoughnessMap;
        Ref<Texture> m_OcclusionMap;
        Ref<Texture> m_EmissiveMap;

        glm::vec3 m_Albedo = glm::vec3(1.0f);
        float m_Metallic = 0.0f;
        float m_Roughness = 0.0f;
        float m_Emisstion = 0.0f;
        float m_Transparency = 0.0f;

        // TODO : Fix this
        bool b_Albedo = true;
        bool b_Metallic = true;
        bool b_Roughness = true;
        bool b_Normal = true;
        bool b_Occlusion = true;
        bool b_Emissive = true;

        friend class MaterialAsset;
    };

}  // namespace Ethereal
