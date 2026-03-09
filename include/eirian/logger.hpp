#pragma once

#include <memory>
#include <spdlog/spdlog.h>
#include <utility>

namespace eirian {
std::shared_ptr<spdlog::logger> getLogger();

namespace log {
template <typename... Args>
inline void info(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    getLogger()->info(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void warn(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    getLogger()->warn(fmt, std::forward<Args>(args)...);
}

template <typename... Args>
inline void error(spdlog::format_string_t<Args...> fmt, Args&&... args) {
    getLogger()->error(fmt, std::forward<Args>(args)...);
}
} // namespace log
}
