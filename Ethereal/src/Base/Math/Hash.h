#pragma once

#include <string>

namespace Ethereal::Math {
class Hash {
   public:
    static constexpr uint32_t GenerateFNVHash(const char* str) {
        constexpr uint32_t FNV_PRIME = 16777619u;
        constexpr uint32_t OFFSET_BASIS = 2166136261u;

        const size_t length = std::string_view(str).length() + 1;
        uint32_t hash = OFFSET_BASIS;
        for (size_t i = 0; i < length; ++i) {
            hash ^= *str++;
            hash *= FNV_PRIME;
        }
        return hash;
    }

    static constexpr uint32_t GenerateFNVHash(std::string_view string) {
        return GenerateFNVHash(string.data());
    }

    static uint32_t CRC32(const char* str);
    static uint32_t CRC32(const std::string& string);

    static uint32_t murmur3(const uint32_t* key, size_t wordCount, uint32_t seed) noexcept {
        uint32_t h = seed;
        size_t i = wordCount;
        do {
            uint32_t k = *key++;
            k *= 0xcc9e2d51u;
            k = (k << 15u) | (k >> 17u);
            k *= 0x1b873593u;
            h ^= k;
            h = (h << 13u) | (h >> 19u);
            h = (h * 5u) + 0xe6546b64u;
        } while (--i);
        h ^= wordCount;
        h ^= h >> 16u;
        h *= 0x85ebca6bu;
        h ^= h >> 13u;
        h *= 0xc2b2ae35u;
        h ^= h >> 16u;
        return h;
    }

    template <typename T>
    struct MurmurHashFn {
        uint32_t operator()(const T& key) const noexcept {
            static_assert(0 == (sizeof(key) & 3u),
                          "Hashing requires a size that is a multiple of 4.");
            return murmur3((const uint32_t*)&key, sizeof(key) / 4, 0);
        }
    };
};

}  // namespace Ethereal::Math
