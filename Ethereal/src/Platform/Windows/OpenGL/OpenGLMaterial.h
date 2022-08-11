#pragma once
#include "Renderer/Material.h"
namespace Ethereal
{
    class OpenGLMaterial : public Material {
      public:
        OpenGLMaterial(const std::string& name = "");
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

        virtual const std::string& GetName() const override { return m_Name; }

      private:
        std::string m_Name;
        uint32_t m_MaterialFlags = 0;
    };

}  // namespace Ethereal
