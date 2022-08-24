#pragma once
//#include "pch.h"
#include <string>
#include <Asset/Asset.h>
#include "Core/Utils.h"
#include "Utils/Macro.h"

namespace Ethereal
{
    class TextureData : public RefCounted {
      public:
        uint32_t m_width{0};
        uint32_t m_height{0};
        uint32_t m_depth{0};
        uint32_t m_mip_levels{0};
        void* m_pixels{nullptr};

        ETHEREAL_PIXEL_FORMAT m_format{ETHEREAL_PIXEL_FORMAT::UNKNOWN};
        ETHEREAL_IMAGE_TYPE m_type{ETHEREAL_IMAGE_TYPE::ETHEREAL_IMAGE_TYPE_UNKNOWM};

        ETHEREAL_WARP_FORMAT m_warp_format{ETHEREAL_WARP_FORMAT::ETHEREAL_WARP_FORMAT_CLAMP_TO_EDGE};
        ETHEREAL_FILTER_FORMAT m_filter_format{ETHEREAL_FILTER_FORMAT::ETHEREAL_FILTER_FORMAT_LINEAR};

        TextureData() = default;
        ~TextureData() {
            if (m_pixels) {
                free(m_pixels);
            }
        }
        bool isValid() const { return m_pixels != nullptr; }
    };

    class Texture : public Asset {
      public:
        virtual ~Texture() = default;

        virtual uint32_t GetWidth() const = 0;
        virtual uint32_t GetHeight() const = 0;

        static AssetType GetStaticType() { return AssetType::Texture; }
        virtual AssetType GetAssetType() const override { return GetStaticType(); }

        virtual void Bind(uint32_t slot = 0) const = 0;
        virtual bool operator==(const Texture& other) const = 0;
        virtual void SetData(void* data, uint32_t size) = 0;
        virtual void Clear(int value) = 0;
        virtual uint32_t GetRendererID() const = 0;
        virtual bool IsLoaded() const = 0;

        virtual void GenerateMipmaps() const = 0;
    };

    class Texture2D : public Texture {
      public:
        virtual void BindToFramebuffer(uint32_t attachmentid, uint32_t miplevel = 0) const = 0;
        virtual void BindImage(uint32_t slot, uint32_t miplevel = 0) const = 0;
        static Ref<Texture2D> Create(const std::string& path);
        static Ref<Texture2D> Create(const Ref<TextureData>& data);
    };

    class TextureCube : public Texture {
      public:
        virtual void BindToFramebuffer(uint32_t attachmentid, uint32_t face = 0, uint32_t miplevel = 0) const = 0;
        static Ref<TextureCube> Create(std::vector<std::string>& paths);
        static Ref<TextureCube> Create(const Ref<TextureData>& data);
    };

    class Texture3D : public Texture {
      public:
        virtual void BindToFramebufferDepth(uint32_t attachmentid) const = 0;
        static Ref<Texture3D> Create(const Ref<TextureData>& data);
    };
}  // namespace Ethereal
