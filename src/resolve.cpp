#include "yonaa/resolve.hpp"

#include "yonaa/detail/getaddrinfo.hpp"

namespace yonaa {
resolve_result resolve(const std::string &hostname, const std::string &service) {
    // Delegate function call and throw if necessary
    std::error_code ec;
    auto result = resolve(hostname, service, ec);

    if (ec) throw ec;

    return result;
}

resolve_result resolve(
    const std::string &hostname, const std::string &service, std::error_code &ec) {
    gai_result_type *target_info = detail::gai::getaddrinfo(hostname, service);
    if (!target_info) {
        // TODO(Caleb): Error handling here
        ec.assign(3, std::system_category());
        return {};
    }

    // Pack result
    resolve_result result;
    for (gai_result_type *ai = target_info; ai != nullptr; ai = ai->ai_next) {
        auto e = endpoint::from_native_address(ai->ai_protocol, ai->ai_addr, ai->ai_addrlen);
        result.push_back(e);
    }

    freeaddrinfo(target_info);

    return result;
}
}  // namespace yonaa
