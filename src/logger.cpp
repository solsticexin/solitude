#include "eirian/logger.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

namespace eirian {
std::shared_ptr<spdlog::logger> getLogger() {
    static auto logger = [] {
        auto instance = spdlog::stdout_color_mt("eirian");
        instance->set_pattern("%^[%L] %H:%M:%S.%e %n: %v%$");
        instance->set_level(spdlog::level::debug);
        return instance;
    }();
    return logger;
}
} // namespace eirian
