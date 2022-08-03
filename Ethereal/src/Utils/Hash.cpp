#include "pch.h"
#include "Hash.h"

#include <array>
constexpr auto gen_crc32_table() {
    constexpr auto num_bytes = 256;
    constexpr auto num_iterations = 8;
    constexpr auto polynomial = 0xEDB88320;

    auto crc32_table = std::array<uint32_t, num_bytes>{};

    for (auto byte = 0u; byte < num_bytes; ++byte) {
        auto crc = byte;

        for (auto i = 0; i < num_iterations; ++i) {
            auto mask = -(crc & 1);
            crc = (crc >> 1) ^ (polynomial & mask);
        }

        crc32_table[byte] = crc;
    }

    return crc32_table;
}

static constexpr auto crc32_table = gen_crc32_table();
static_assert(crc32_table.size() == 256 && crc32_table[1] == 0x77073096 && crc32_table[255] == 0x2D02EF8D,
              "gen_crc32_table generated unexpected result.");

namespace Ethereal
{

    uint32_t Hash::CRC32(const char* str) {
        auto crc = 0xFFFFFFFFu;

        for (auto i = 0u; auto c = str[i]; ++i) {
            crc = crc32_table[(crc ^ c) & 0xFF] ^ (crc >> 8);
        }

        return ~crc;
    }

    uint32_t Hash::CRC32(const std::string& string) { return CRC32(string.c_str()); }

}  // namespace Ethereal