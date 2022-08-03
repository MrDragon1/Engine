#pragma once
#include "Renderer/Material.h"
namespace Ethereal
{
    class OpenGLMaterial : public Material {
      public:
        OpenGLMaterial(const Ref<Shader>& shader, const std::string& name = "");
        OpenGLMaterial(Ref<Material> material, const std::string& name = "");
        virtual ~OpenGLMaterial() override;

        virtual uint32_t GetFlags() const override { return m_MaterialFlags; }
        virtual void SetFlags(uint32_t flags) override { m_MaterialFlags = flags; }
        virtual bool GetFlag(MaterialFlag flag) const override { return (uint32_t)flag & m_MaterialFlags; }
        virtual void SetFlag(MaterialFlag flag, bool value = true) override {
            if (value) {
                m_MaterialFlags |= (uint32_t)flag;
            } else {
                m_MaterialFlags &= ~(uint32_t)flag;
            }
        }

        virtual Ref<Shader> GetShader() override { return m_Shader; }
        virtual const std::string& GetName() const override { return m_Name; }

      private:
        Ref<Shader> m_Shader;
        std::string m_Name;

        enum class PendingDescriptorType
        {
            None = 0,
            Texture2D,
            TextureCube
        };
        uint32_t m_MaterialFlags = 0;

        std::vector<Ref<Texture>> m_Textures;  // TODO: Texture should only be stored as images
        std::vector<std::vector<Ref<Texture>>> m_TextureArrays;
    };

}  // namespace Ethereal
