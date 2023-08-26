#pragma once

#include "yonaa/endpoint.hpp"
#include "yonaa/resolve.hpp"
#include "yonaa/types.hpp"

namespace yonaa::detail::socket_ops {

/// @brief Return a socket that is connected to the specified endpoint, or 0 if the connection could
/// not be established.
/// @param remote_endpoints The remote address to connect to.
/// @return A socket that is connected to the specified endpoint, or 0 if the connection could not
/// be established.
socket_type create_connected_socket(const resolve_result &remote_endpoints);

/// @brief Return a socket that is primed to accept incoming connections at the local endpoint
/// provided, or 0 if such a socket could not be created.
/// @param local_endpoints The local address to wait for incoming connections at.
/// @param backlog_size The maximum size of the incoming connection backlog associated with this
/// socket.
/// @param reuse_addr True if this socket should use the SO_REUSEADDR option when configuring its
/// socket. (see: man 7 ip).
/// @return A socket that is primed to accept incoming connections at the local endpoint provided,
/// or 0 if such a socket could not be created.
socket_type create_listening_socket(
    const resolve_result &local_endpoints, uint64_t backlog_size, bool reuse_addr = false);

/// @brief Gracefully close an open socket.
/// @param socket_fd The socket to close.
void close_socket(socket_type socket_fd);

/// @brief Return an endpoint representing the local end of this socket.
/// @param socket_fd The socket to query.
/// @return An endpoint representing the local end of this socket.
endpoint get_local_endpoint(socket_type socket_fd);

/// @brief Return an endpoint representing the remote end of this socket.
/// @param socket_fd The socket to query.
/// @return An endpoint representing the remote end of this socket.
endpoint get_remote_endpoint(socket_type socket_fd);

}  // namespace yonaa::detail::socket_ops
