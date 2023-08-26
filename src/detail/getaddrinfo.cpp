#include "yonaa/detail/getaddrinfo.hpp"

#include <cstring>

#include "yonaa/addresses.hpp"

namespace yonaa::detail::gai {

gai_result_type *getaddrinfo(std::string hostname, std::string service) {
    bool use_inaddr_any = (hostname == any_address);

    // Get remote address info
    addrinfo hints;
    std::memset(&hints, 0, sizeof(hints));
    hints.ai_family   = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags    = AI_ADDRCONFIG | ((use_inaddr_any) ? AI_PASSIVE : 0);

    addrinfo *target_info;
    const char *gai_name = (use_inaddr_any) ? NULL : hostname.c_str();
    int gai_result       = getaddrinfo(gai_name, service.c_str(), &hints, &target_info);
    if (gai_result != 0) return NULL;  // TODO(Caleb): Rethink this particular error handling case.

    return target_info;
}

}  // namespace yonaa::detail::gai
