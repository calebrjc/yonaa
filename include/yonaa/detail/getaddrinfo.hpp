#pragma once

#include <string>

#include "yonaa/types.hpp"

namespace yonaa::detail::gai {

/// @brief Return the addrinfo * from a call to getaddrinfo() with the name and service specified.
/// The pointer will have to be freed with freeaddrinfo().
/// @param hostname The hostname to be resolved.
/// @param service The service to be resolved.
/// @return The addrinfo * from a call to getaddrinfo() with the name and service specified.
gai_result_type *getaddrinfo(std::string hostname, std::string service);

}  // namespace yonaa::detail::gai
