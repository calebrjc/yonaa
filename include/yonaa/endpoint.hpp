#pragma once

#include <string>

#include "yonaa/buffer.hpp"
#include "yonaa/types.hpp"

namespace yonaa {

/// @brief A unit of networking, used to store the address of a host computer.
struct endpoint {
    /// @brief Create an empty endpoint.
    endpoint();

    /// @brief Return an endpoint created from a native address structure.
    /// @param protocol The protocol associated with the native address structure. (see: man 7 ip)
    /// @param addr The native address structure.
    /// @param addr_size The size (in bytes) of the native address structure.
    /// @return An endpoint created from a native address structure.
    static endpoint from_native_address(
        protocol_type protocol, address_type *addr, address_size_type addr_size);

    /// @brief Return the identifier of the address family associated with this socket.
    /// @return The identifier of the address family associated with this socket.
    address_family_type family() const;

    /// @brief Return the identifier of the protocol associated with this socket. (see: man 7 ip)
    /// @return The identifier of the protocol associated with this socket. (see: man 7 ip)
    protocol_type protocol() const;

    /// @brief Return a pointer to the underlying native storage for this endpoint.
    /// @return A pointer to the underlying native storage for this endpoint.
    address_type *data();

    /// @brief Return a pointer to the underlying native storage for this endpoint.
    /// @return A pointer to the underlying native storage for this endpoint.
    const address_type *data() const;

    /// @brief Return the size of the native address stored by this endpoint.
    /// @return The size of the native address stored by this endpoint.
    address_size_type size() const;

    /// @brief Return the IP address referred to by this endpoint in string form.
    /// @return The IP address referred to by this endpoint in string form.
    std::string addr() const;

    /// @brief Return the port number referred to by this endpoint in string form.
    /// @return The port number referred to by this endpoint in string form.
    std::string port() const;

    /// @brief Return a string representation of this endpoint.
    /// @return A string representation of this endpoint.
    std::string str() const;

    /// @brief Return true if the other endpoint can be considered equal to this one, and false
    /// otherwise.
    /// @param other The other endpoint to be compared.
    /// @return True if the other endpoint can be considered equal to this one, and false otherwise.
    bool operator==(const endpoint &other) const;

   private:
    protocol_type protocol_;
    buffer storage_;
};
}  // namespace yonaa
