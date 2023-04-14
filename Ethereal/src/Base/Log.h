#pragma once
#include "Utils/EngineMarco.h"

#include "spdlog/spdlog.h"
#include "spdlog/fmt/ostr.h"

#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/string_cast.hpp"
namespace Ethereal {
class Log {
   public:
    static void Init();
    inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
    inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

   private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
}  // namespace Ethereal

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
#define ET_CORE_TRACE(...) ::Ethereal::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define ET_CORE_INFO(...) ::Ethereal::Log::GetCoreLogger()->info(__VA_ARGS__)
#define ET_CORE_WARN(...) ::Ethereal::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define ET_CORE_ERROR(...) ::Ethereal::Log::GetCoreLogger()->error(__VA_ARGS__)
#define ET_CORE_FATAL(...) ::Ethereal::Log::GetCoreLogger()->fatal(__VA_ARGS__)

// Client log macros
#define ET_TRACE(...) ::Ethereal::Log::GetClientLogger()->trace(__VA_ARGS__)
#define ET_INFO(...) ::Ethereal::Log::GetClientLogger()->info(__VA_ARGS__)
#define ET_WARN(...) ::Ethereal::Log::GetClientLogger()->warn(__VA_ARGS__)
#define ET_ERROR(...) ::Ethereal::Log::GetClientLogger()->error(__VA_ARGS__)
#define ET_FATAL(...) ::Ethereal::Log::GetClientLogger()->fatal(__VA_ARGS__)

#define SPDLOG_LOGGER_INFO(logger, ...) SPDLOG_LOGGER_CALL(logger, spdlog::level::info, __VA_ARGS__)
#define ET_TEST_INFO(...) SPDLOG_LOGGER_INFO(::Ethereal::Log::GetCoreLogger(), __VA_ARGS__)
