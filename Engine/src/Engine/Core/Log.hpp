#pragma once
#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"
#include "Engine/Core/Utils.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
namespace Engine
{
    class Log {
      public:
        static void Init();
        inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
        inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

      private:
        static std::shared_ptr<spdlog::logger> s_CoreLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };
}  // namespace Engine

template <typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& vector) {
    return os << glm::to_string(vector);
}

template <typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& matrix) {
    return os << glm::to_string(matrix);
}

template <typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, glm::qua<T, Q> quaternio) {
    return os << glm::to_string(quaternio);
}

// Core log macros
#define ENGINE_CORE_TRACE(...) ::Engine::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ENGINE_CORE_INFO(...) ::Engine::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ENGINE_CORE_WARN(...) ::Engine::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ENGINE_CORE_ERROR(...) ::Engine::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ENGINE_CORE_FATAL(...) ::Engine::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define ENGINE_TRACE(...) ::Engine::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ENGINE_INFO(...) ::Engine::Log::GetClientLogger()->info(__VA_ARGS__)
#define ENGINE_WARN(...) ::Engine::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ENGINE_ERROR(...) ::Engine::Log::GetClientLogger()->error(__VA_ARGS__)
#define ENGINE_FATAL(...) ::Engine::Log::GetClientLogger()->fatal(__VA_ARGS__)
