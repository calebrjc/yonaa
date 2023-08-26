#pragma once

#include <string>

namespace yonaa {

/// @brief The hostname used to refer to the host computer in calls to net::resolve() to produce a
/// socket bindable to all local interfaces. (see: INADDR_ANY).
const std::string any_address = "0.0.0.0";

/// @brief The hostname used to refer to the host computer in calls to net::resolve() to connect
/// to a local socket.
const std::string loopback_address = "127.0.0.1";

}  // namespace yonaa
