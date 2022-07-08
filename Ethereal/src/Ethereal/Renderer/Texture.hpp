#pragma once
#include "pch.hpp"
#include <string>
#include "Ethereal/Core/Utils.hpp"
#include "Ethereal/Utils/Macro.hpp"

namespace Ethereal
{
    class TextureData
    {
    public:
        uint32_t m_width {0};
        uint32_t m_height {0};
        uint32_t m_depth {0};
        uint32_t m_mip_levels {0};
        uint32_t m_array_layers {0};
        void*    m_pixels {nullptr};

        ETHEREAL_PIXEL_FORMAT m_format {ETHEREAL_PIXEL_FORMAT::ETHEREAL_PIXEL_FORMAT_UNKNOWN};
        ETHEREAL_IMAGE_TYPE   m_type {ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_UNKNOWM};

        TextureData() = default;
        ~TextureData()
        {
            if (m_pixels)
            {
                free(m_pixels);
            }
        }
        bool isValid() const { return m_pixels != nullptr; }
    };

    class Texture {
      public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual bool operator==(const Texture& other) const = 0;
        virtual void SetData(void* data, uint32_t size) = 0;
        virtual uint32_t GetRendererID() const = 0; 
        virtual bool IsLoaded() const = 0;
    };

    class Texture2D : public Texture {
      public:
        static Ref<Texture2D> Create(const std::string& path);
        static Ref<Texture2D> Create(const Ref<TextureData>& data);
    };
}  // namespace Ethereal
