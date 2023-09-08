#include "yonaa/logging.hpp"

#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

namespace yonaa::logging {

namespace detail {

std::shared_ptr<spdlog::logger> internal_logger_ = nullptr;

std::shared_ptr<spdlog::logger> app_logger_ = nullptr;

}  // namespace detail

void init(log_level log_level) {
    spdlog::set_level(static_cast<spdlog::level::level_enum>(log_level));

    detail::internal_logger = spdlog::stdout_color_mt("yonaa");
    detail::internal_logger->set_pattern("%^[%T] %n: %v%$");

    detail::app_logger = spdlog::stdout_color_mt("app");
    detail::app_logger->set_pattern("%^[%T] %n: %v%$");
}

}  // namespace yonaa::logging
