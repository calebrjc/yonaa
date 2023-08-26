#include "yonaa/endpoint.hpp"

#include <arpa/inet.h>

#include <sstream>

#include "yonaa/detail/sockaddr_ops.hpp"

namespace yonaa {

endpoint::endpoint() : protocol_(0), storage_(sizeof(sockaddr_storage)) {}

endpoint endpoint::from_native_address(
    protocol_type protocol, address_type *addr, address_size_type addr_size) {
    endpoint e;
    e.protocol_ = protocol;
    e.storage_  = buffer(addr_size);

    std::memcpy(e.data(), addr, addr_size);

    return e;
}

address_family_type endpoint::family() const {
    return data()->sa_family;
}

protocol_type endpoint::protocol() const {
    return protocol_;
}

address_type *endpoint::data() {
    return storage_.as<address_type>();
}

const address_type *endpoint::data() const {
    return storage_.as<address_type>();
}

address_size_type endpoint::size() const {
    return storage_.size();
}

std::string endpoint::addr() const {
    buffer buffer(INET6_ADDRSTRLEN);
    inet_ntop(AF_INET, detail::sockaddr_ops::get_in_addr(data()), buffer.data(), buffer.size());

    switch (data()->sa_family) {
        case AF_INET:
            return std::string(buffer.data(), INET_ADDRSTRLEN);
        case AF_INET6:
            return std::string(buffer.data(), INET6_ADDRSTRLEN);
        default:
            return "INVALID";
    }
}

std::string endpoint::port() const {
    switch (data()->sa_family) {
        case AF_INET:
        case AF_INET6:
            return std::to_string((int)detail::sockaddr_ops::get_port(data()));
        default:
            return "INVALID";
    }
}

std::string endpoint::str() const {
    std::stringstream ss;
    switch (data()->sa_family) {
        case AF_INET:
            ss << addr() << ":" << port();
            return ss.str();
        case AF_INET6:
            ss << "[" << addr() << "]:" << port();
            return ss.str();
        default:
            return "INVALID:INVALID";
    }
}

bool endpoint::operator==(const endpoint &other) const {
    return memcmp(data(), other.data(), size()) == 0;
}

}  // namespace yonaa
