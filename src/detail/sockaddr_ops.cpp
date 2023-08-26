#include "yonaa/detail/sockaddr_ops.hpp"

namespace yonaa::detail::sockaddr_ops {

void *get_in_addr(const address_type *addr) {
    if (addr->sa_family == AF_INET) { return &(((sockaddr_in *)addr)->sin_addr); }

    return &(((sockaddr_in6 *)addr)->sin6_addr);
}

uint16_t get_port(const address_type *addr) {
    // Both sockaddr_in and sockaddr_in6 begin with (where "x" is "" or "6")
    //
    // struct sockaddr_inx {
    //     uint16_t sinx_family;
    //     uint16_t sinx_port
    // }
    //
    // Meaning that this access will return the same value for both types. Therefore, only one
    // implementation is used.
    return ntohs(((sockaddr_in *)addr)->sin_port);
}

}  // namespace yonaa::detail::sockaddr_ops
