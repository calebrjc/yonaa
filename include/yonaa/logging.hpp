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

/// @brief A pointer to the internal logger associated with yonaa
static std::shared_ptr<spdlog::logger> internal_logger;

/// @brief A pointer to the application logger associated with yonaa
static std::shared_ptr<spdlog::logger> app_logger;

}  // namespace detail

/// @brief Initialize yonaa's logging system
/// @param log_level The log level to use
void init(log_level log_level = log_level::info);

}  // namespace logging

}  // namespace yonaa

#if defined(YONAA_ENABLE_LOGGING)

#define YONAA_TRACE(...)                                          \
    if (::yonaa::logging::detail::app_logger) {                   \
        ::yonaa::logging::detail::app_logger->trace(__VA_ARGS__); \
    }

#define YONAA_DEBUG(...)                                          \
    if (::yonaa::logging::detail::app_logger) {                   \
        ::yonaa::logging::detail::app_logger->debug(__VA_ARGS__); \
    }

#define YONAA_INFO(...)                                          \
    if (::yonaa::logging::detail::app_logger) {                  \
        ::yonaa::logging::detail::app_logger->info(__VA_ARGS__); \
    }

#define YONAA_WARN(...)                                          \
    if (::yonaa::logging::detail::app_logger) {                  \
        ::yonaa::logging::detail::app_logger->warn(__VA_ARGS__); \
    }

#define YONAA_ERROR(...)                                          \
    if (::yonaa::logging::detail::app_logger) {                   \
        ::yonaa::logging::detail::app_logger->error(__VA_ARGS__); \
    }

#define YONAA_CRITICAL(...)                                          \
    if (::yonaa::logging::detail::app_logger) {                      \
        ::yonaa::logging::detail::app_logger->critical(__VA_ARGS__); \
    }

#define YONAA_INTERNAL_TRACE(...)                                      \
    if (::yonaa::logging::detail::internal_logger) {                   \
        ::yonaa::logging::detail::internal_logger->trace(__VA_ARGS__); \
    }

#define YONAA_INTERNAL_DEBUG(...)                                      \
    if (::yonaa::logging::detail::internal_logger) {                   \
        ::yonaa::logging::detail::internal_logger->debug(__VA_ARGS__); \
    }

#define YONAA_INTERNAL_INFO(...)                                      \
    if (::yonaa::logging::detail::internal_logger) {                  \
        ::yonaa::logging::detail::internal_logger->info(__VA_ARGS__); \
    }

#define YONAA_INTERNAL_WARN(...)                                      \
    if (::yonaa::logging::detail::internal_logger) {                  \
        ::yonaa::logging::detail::internal_logger->warn(__VA_ARGS__); \
    }

#define YONAA_INTERNAL_ERROR(...)                                      \
    if (::yonaa::logging::detail::internal_logger) {                   \
        ::yonaa::logging::detail::internal_logger->error(__VA_ARGS__); \
    }

#define YONAA_INTERNAL_CRITICAL(...)                                      \
    if (::yonaa::logging::detail::internal_logger) {                      \
        ::yonaa::logging::detail::internal_logger->critical(__VA_ARGS__); \
    }

#else

#define YONAA_TRACE(...)    (void)0
#define YONAA_DEBUG(...)    (void)0
#define YONAA_INFO(...)     (void)0
#define YONAA_WARN(...)     (void)0
#define YONAA_ERROR(...)    (void)0
#define YONAA_CRITICAL(...) (void)0

#define YONAA_INTERNAL_TRACE(...)    (void)0
#define YONAA_INTERNAL_DEBUG(...)    (void)0
#define YONAA_INTERNAL_INFO(...)     (void)0
#define YONAA_INTERNAL_WARN(...)     (void)0
#define YONAA_INTERNAL_ERROR(...)    (void)0
#define YONAA_INTERNAL_CRITICAL(...) (void)0

#endif
