#pragma once

#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#pragma warning(pop)

namespace yonaa {

enum class log_level {
    trace    = SPDLOG_LEVEL_TRACE,
    debug    = SPDLOG_LEVEL_DEBUG,
    info     = SPDLOG_LEVEL_INFO,
    warn     = SPDLOG_LEVEL_WARN,
    err      = SPDLOG_LEVEL_ERROR,
    critical = SPDLOG_LEVEL_CRITICAL,
    off      = SPDLOG_LEVEL_OFF
};

namespace logging {

namespace detail {

/// @brief The name of the application initializing the logging system.
inline std::string app_name;

}  // namespace detail

/// @brief Initialize yonaa's logging system.
/// @param app_name The name of the application initializing the logging system.
/// @param log_level The log level to use.
void init(std::string_view app_name = "app", log_level log_level = log_level::trace);

}  // namespace logging

}  // namespace yonaa

#if defined(YONAA_ENABLE_LOGGING)

#define YONAA_LOG(logger_name, level, ...) \
    if (::spdlog::get(logger_name)) {::spdlog::get(logger_name)->level(__VA_ARGS__); }

#define YONAA_INTERNAL_TRACE(...)    YONAA_LOG("yonaa", trace, __VA_ARGS__)
#define YONAA_INTERNAL_DEBUG(...)    YONAA_LOG("yonaa", debug, __VA_ARGS__)
#define YONAA_INTERNAL_INFO(...)     YONAA_LOG("yonaa", info, __VA_ARGS__)
#define YONAA_INTERNAL_WARN(...)     YONAA_LOG("yonaa", warn, __VA_ARGS__)
#define YONAA_INTERNAL_ERROR(...)    YONAA_LOG("yonaa", error, __VA_ARGS__)
#define YONAA_INTERNAL_CRITICAL(...) YONAA_LOG("yonaa", critical, __VA_ARGS__)

#define YONAA_TRACE(...)    YONAA_LOG(::yonaa::logging::detail::app_name, trace, __VA_ARGS__)
#define YONAA_DEBUG(...)    YONAA_LOG(::yonaa::logging::detail::app_name, debug, __VA_ARGS__)
#define YONAA_INFO(...)     YONAA_LOG(::yonaa::logging::detail::app_name, info, __VA_ARGS__)
#define YONAA_WARN(...)     YONAA_LOG(::yonaa::logging::detail::app_name, warn, __VA_ARGS__)
#define YONAA_ERROR(...)    YONAA_LOG(::yonaa::logging::detail::app_name, error, __VA_ARGS__)
#define YONAA_CRITICAL(...) YONAA_LOG(::yonaa::logging::detail::app_name, critical, __VA_ARGS__)

#else

#define YONAA_LOG(logger_name, level, ...) (void)0

#define YONAA_INTERNAL_TRACE(...)    (void)0
#define YONAA_INTERNAL_DEBUG(...)    (void)0
#define YONAA_INTERNAL_INFO(...)     (void)0
#define YONAA_INTERNAL_WARN(...)     (void)0
#define YONAA_INTERNAL_ERROR(...)    (void)0
#define YONAA_INTERNAL_CRITICAL(...) (void)0

#define YONAA_TRACE(...)    (void)0
#define YONAA_DEBUG(...)    (void)0
#define YONAA_INFO(...)     (void)0
#define YONAA_WARN(...)     (void)0
#define YONAA_ERROR(...)    (void)0
#define YONAA_CRITICAL(...) (void)0

#endif
