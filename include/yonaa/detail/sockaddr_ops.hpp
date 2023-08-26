#pragma once

#include "yonaa/types.hpp"

namespace yonaa::detail::sockaddr_ops {

/// @brief Return the in_addr or in6_addr associated with the address contained in sa.
/// @param addr The address to be inspected.
/// @return The in_addr or in6_addr associated with the address contained in sa.
void *get_in_addr(const address_type *addr);

/// @brief Return the port number associated with the address contained in sa.
/// @param addr The address to be inspected.
/// @return The port number associated with the address contained in sa.
uint16_t get_port(const address_type *addr);

}  // namespace yonaa::detail::sockaddr_ops
