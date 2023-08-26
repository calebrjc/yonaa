#pragma once

#include <system_error>
#include <vector>

#include "yonaa/endpoint.hpp"

namespace yonaa {
/// @brief The type returned by a call to `net::resolve()`.
using resolve_result = std::vector<endpoint>;

/// @brief Return the result of name resolution for the given hostname and service.
/// @param hostname The IP (v4 or v6) address of the desired host, or their canonical name.
/// @param service The name of the desired service or its corresponding port number, in string form.
/// @return The result of name resolution for the given hostname and service.
resolve_result resolve(const std::string &hostname, const std::string &service);

/// @brief Return the result of name resolution for the given hostname and service.
/// @param hostname The IP (v4 or v6) address of the desired host, or their canonical name.
/// @param service The name of the desired service or its corresponding port number, in string form.
/// @param ec An error_code that is set if an error occurs.
/// @return The result of name resolution for the given hostname and service.
resolve_result resolve(
    const std::string &hostname, const std::string &service, std::error_code &ec);
}  // namespace yonaa
