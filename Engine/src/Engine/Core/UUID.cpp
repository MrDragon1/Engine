#include "pch.hpp"
#include "UUID.hpp"

#include <unordered_map>
#include <random>

namespace Engine
{
    static std::random_device s_RandomDevice;
    static std::mt19937_64 s_Engine(s_RandomDevice());
    static std::uniform_int_distribution<uint64_t> s_UniformDistribution;

    UUID::UUID() : m_UUID(s_UniformDistribution(s_RandomDevice)) {}

    UUID::UUID(uint64_t uuid) : m_UUID(uuid) {}
}  // namespace Engine