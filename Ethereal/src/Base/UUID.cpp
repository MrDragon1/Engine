#include "pch.h"
#include "UUID.h"

#include <unordered_map>
#include <random>

namespace Ethereal
{
    static std::random_device sRandomDevice;
    static std::mt19937_64 sEngine(sRandomDevice());
    static std::uniform_int_distribution<uint64_t> sUniformDistribution;

    UUID::UUID() : mUUID(sUniformDistribution(sRandomDevice)) {}

    UUID::UUID(uint64_t uuid) : mUUID(uuid) {}
}  // namespace Ethereal