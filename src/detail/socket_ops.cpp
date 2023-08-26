#include "yonaa/detail/socket_ops.hpp"

#include <unistd.h>

namespace yonaa::detail::socket_ops {

namespace detail {

endpoint get_endpoint(socket_type socket_fd, bool local_socket) {
    // Get address
    address_storage_type ss;
    address_size_type ss_size = sizeof(ss);

    int getname_result = 0;

    if (local_socket) {
        getname_result = ::getsockname(socket_fd, (address_type *)&ss, &ss_size);
    } else {
        getname_result = ::getpeername(socket_fd, (address_type *)&ss, &ss_size);
    }

    //? Note(Caleb): We should probably check for errors here. Maybe use error_code?
    (void)getname_result;

    // Get socket protocol
    int protocol                    = 0;
    address_size_type protocol_size = sizeof(protocol);
    int gso_result = getsockopt(socket_fd, SOL_SOCKET, SO_PROTOCOL, &protocol, &protocol_size);

    //? Note(Caleb): We should probably check for errors here. Maybe use error_code?
    (void)gso_result;

    return endpoint::from_native_address(protocol, (address_type *)&ss, ss_size);
}

}  // namespace detail

socket_type create_connected_socket(const resolve_result &remote_endpoints) {
    for (const endpoint &e : remote_endpoints) {
        int socket_fd = ::socket(e.family(), SOCK_STREAM, e.protocol());
        if (socket_fd == -1) continue;

        int connect_result = ::connect(socket_fd, e.data(), e.size());
        if (connect_result == -1) continue;

        return socket_fd;
    }

    return 0;
}

socket_type create_listening_socket(
    const resolve_result &local_endpoints, uint64_t backlog_size, bool reuse_addr) {
    for (const endpoint &e : local_endpoints) {
        // Attempt to get a socket handle
        int socket_fd = ::socket(e.family(), SOCK_STREAM, e.protocol());
        if (socket_fd == -1) continue;

        // Enable SO_REUSEADDR if necessary
        if (reuse_addr) {
            int on         = 1;
            int sso_result = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
            if (sso_result == -1) continue;
        }

        int bind_result = ::bind(socket_fd, e.data(), e.size());
        if (bind_result == -1) continue;

        int listen_result = ::listen(socket_fd, backlog_size);
        if (listen_result == -1) continue;

        return socket_fd;
    }

    return 0;
}

endpoint get_local_endpoint(socket_type socket_fd) {
    return detail::get_endpoint(socket_fd, true);
}

endpoint get_remote_endpoint(socket_type socket_fd) {
    return detail::get_endpoint(socket_fd, false);
}

void close_socket(socket_type socket_fd) {
    // Note(Caleb): shutdown() does not fail meaningfully for our use cases, so we don't do any
    // error checking here.
    ::shutdown(socket_fd, SHUT_RDWR);

    // Note(Caleb): See above comment.
    ::close(socket_fd);
}

}  // namespace yonaa::detail::socket_ops
