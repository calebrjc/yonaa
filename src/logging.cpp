#include "yonaa/logging.hpp"

#pragma warning(push, 0)
#include <spdlog/sinks/stdout_color_sinks.h>
#pragma warning(pop)

namespace yonaa::logging {

void init(std::string_view app_name, log_level log_level) {
    auto internal_logger = spdlog::stdout_color_mt("yonaa");
    internal_logger->set_pattern("%^[%T] %n: %v%$");
    internal_logger->set_level(static_cast<spdlog::level::level_enum>(log_level));

    detail::app_name = app_name.data();
    auto app_logger = spdlog::stdout_color_mt(app_name.data());
    app_logger->set_pattern("%^[%T] %n: %v%$");
    app_logger->set_level(static_cast<spdlog::level::level_enum>(log_level));
}

}  // namespace yonaa::logging
