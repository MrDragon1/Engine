#pragma once
#include <xhash>
#include "Base/Meta/Reflection.h"
namespace Ethereal
{
    REFLECTION_TYPE(UUID)
    CLASS(UUID, Fields){
      public:
        UUID();
        UUID(uint64_t uuid);
        UUID(const UUID&) = default;
        operator uint64_t() const { return m_UUID; };

        uint64_t m_UUID;
    };

}  // namespace Ethereal

namespace std
{

    template <>
    struct hash<Ethereal::UUID> {
        std::size_t operator()(const Ethereal::UUID& uuid) const { return (uint64_t)uuid; }
    };

}  // namespace std