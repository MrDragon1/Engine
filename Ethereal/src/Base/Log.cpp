#include "Log.h"
#include "spdlog/logger.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <memory>

namespace Ethereal {
std::shared_ptr<spdlog::logger> Log::sCoreLogger;
std::shared_ptr<spdlog::logger> Log::sClientLogger;

void Log::Init() {
    spdlog::set_pattern("%^[%T] %n: %v%$");
    sCoreLogger = spdlog::stdout_color_mt("Engine");
    sCoreLogger->set_level(spdlog::level::trace);

    sClientLogger = spdlog::stdout_color_mt("APP");
    sClientLogger->set_level(spdlog::level::trace);
}
}  // namespace Ethereal
