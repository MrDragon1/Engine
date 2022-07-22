#pragma once
#include <type_traits>
namespace Ethereal
{
    template <typename T>
    class Singleton {
      protected:
        Singleton() = default;

      public:
        [[nodiscard]] static T& GetInstance() noexcept(std::is_nothrow_constructible<T>::value) {
            static T instance;
            return instance;
        }
        virtual ~Singleton() noexcept = default;
        Singleton(const Singleton&) = delete;
        Singleton& operator=(const Singleton&) = delete;
    };
}  // namespace Ethereal