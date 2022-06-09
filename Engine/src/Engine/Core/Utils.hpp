#pragma once

#include "pch.hpp"

#ifdef ENGINE_ENABLE_ASSERTS
#define ENGINE_ASSERT(x, ...)                                   \
    {                                                           \
        if (!(x)) {                                             \
            ENGINE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
        }                                                       \
    }
#define ENGINE_CORE_ASSERT(x, ...)                                   \
    {                                                                \
        if (!(x)) {                                                  \
            ENGINE_CORE_ERROR("Assertion Failed: {0}", __VA_ARGS__); \
        }                                                            \
    }
#else
#define ENGINE_ASSERT(x, ...)
#define ENGINE_CORE_ASSERT(x, ...)
#endif

#define BIT(x) (1 << x)

#define ENGINE_BIND_EVENT_FN(fn) std::bind(&fn, this, std::placeholders::_1)

namespace Engine
{
    template <typename T>
    using Scope = std::unique_ptr<T>;
    template <typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args&&... args) {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }
    
    template <typename T>
    using Ref = std::shared_ptr<T>;
    template <typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args) {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}  // namespace Engine