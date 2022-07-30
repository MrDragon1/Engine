#pragma once

#include "pch.hpp"

#ifdef ET_ENABLE_ASSERTS
#define ET_ASSERT(x, ...)                                   \
    {                                                           \
        if (!(x)) {                                             \
            ET_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
        }                                                       \
    }
#define ET_CORE_ASSERT(x, ...)                                   \
    {                                                                \
        if (!(x)) {                                                  \
            ET_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
        }                                                            \
    }
#else
#define ET_ASSERT(x, ...)
#define ET_CORE_ASSERT(x, ...)
#endif

#define ET_EXPAND_MACRO(x) x
// https://gcc.gnu.org/onlinedocs/cpp/Stringizing.html
#define ET_XSTRINGIFY_MACRO(x) ET_STRINGIFY_MACRO(x)
#define ET_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define ET_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Ethereal
{
    template <typename T>
    using Scope = std::unique_ptr<T>;
    template <typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
}  // namespace Ethereal