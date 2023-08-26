#pragma once

#include <system_error>

#include "bitmask/bitmask.hpp"
#include "yonaa/buffer.hpp"
#include "yonaa/endpoint.hpp"
#include "yonaa/resolve.hpp"
#include "yonaa/types.hpp"

namespace yonaa {

/// @brief Flags used to customize calls to connection::send().
enum class send_flags {
    none          = 0x00,
    dont_route    = 0x01,  /// @brief Specifies that the sent data should not be subject to routing.
    end_of_record = 0x02,  /// @brief Specifies that the sent data terminates a record.
};
BITMASK_DEFINE_MAX_ELEMENT(send_flags, end_of_record)
using send_flags_mask = bitmask::bitmask<send_flags>;

/// @brief Flags used to customize calls to connection::receive().
enum class receive_flags {
    none = 0x00,

    peek = 0x01,  /// @brief A flag that specifies that the received data should be returned, but
                  /// not taken off of the input queue.
};
BITMASK_DEFINE_MAX_ELEMENT(receive_flags, peek)
using receive_flags_mask = bitmask::bitmask<receive_flags>;

/// @brief A networking entity that allows communication between the host and another endpoint,
/// local or remote.
class connection {
   public:
    /// @brief Create an unopened connection.
    connection();

    /// @brief Return an open connection created from a connected native socket.
    /// @param socket_fd The open native socket's file descriptor.
    /// @param remote_endpoint An endpoint representing the remote end of the connection.
    /// @return An open connection created from an open native socket.
    static connection from_native_socket(socket_type socket_fd, const endpoint &remote_endpoint);

    /// @brief Cleanup a connection.
    ~connection();

    // Disable copies ------------------------------------------------------------------------------

    connection(const connection &other)            = delete;
    connection &operator=(const connection &other) = delete;

    // ---------------------------------------------------------------------------------------------

    /// @brief Move a connection from another connection.
    /// @param other The other connection.
    connection(connection &&other);

    /// @brief Move a connection from another connection.
    /// @param other The other connection.
    connection &operator=(connection &&other);

   public:
    /// @brief Establish a connection to the resolved remote address.
    /// @param remote_endpoints The resolved remote address that this connection will attempt to
    /// connect to.
    void connect(const resolve_result &remote_endpoints);

    /// @brief Establish a connection to the resolved remote address.
    /// @param remote_endpoints The resolved remote address that this connection will attempt to
    /// connect to.
    /// @param ec An error_code that is set if an error occurs.
    void connect(const resolve_result &remote_endpoints, std::error_code &ec);

    /// @brief Close this connection gracefully.
    void disconnect();

    /// @brief Send the data contained in the given buffer to the remote endpoint of this
    /// connection.
    /// @param data The data to be sent.
    /// @param flags A bitfield of send_flags constants used to customize this call to send().
    void send(const buffer &data, send_flags_mask flags = send_flags::none) const;

    /// @brief Send the data contained in the given buffer to the remote endpoint of this
    /// connection.
    /// @param data The data to be sent.
    /// @param ec An error_code that is set if an error occurs.
    /// @param flags A bitfield of send_flags constants used to customize this call to send().
    void send(
        const buffer &data, std::error_code &ec, send_flags_mask flags = send_flags::none) const;

    /// @brief Return a buffer containing data sent from the remote endpoint of this connection. If
    /// the remote end of this connection is disconnected, then the buffer will be empty and this
    /// connection will return to a closed state.
    /// @return A buffer containing data sent from the remote endpoint of this connection.
    /// @param flags A bitfield of receive_flags constants used to customize this call to
    /// receive().
    buffer receive(receive_flags_mask flags = receive_flags::none);

    /// @brief Return a buffer containing size bytes of data sent from the remote endpoint of this
    /// connection. If the remote end of this connection is disconnected, then the buffer will be
    /// empty and this connection will return to a closed state.
    /// @return A buffer containing data sent from the remote endpoint of this connection.
    //  @param size The number of bytes to be received.
    /// @param flags A bitfield of receive_flags constants used to customize this call to
    /// receive().
    buffer receive(size_t size, receive_flags_mask flags = receive_flags::none);

    /// @brief Return a buffer containing data sent from the remote endpoint of this connection. If
    /// the remote end of this connection is disconnected, or an error occurs, then the buffer will
    /// be empty and this connection will return to a closed state.
    /// @return A buffer containing data sent from the remote endpoint of this connection.
    //  @param size The number of bytes to be received.
    /// @param ec An error_code that is set if an error occurs.
    /// @param flags A bitfield of receive_flags constants used to customize this call to
    /// receive().
    buffer receive(std::error_code &ec, receive_flags_mask flags = receive_flags::none);

    /// @brief Return a buffer containing size bytes of data sent from the remote endpoint of this
    /// connection. If the remote end of this connection is disconnected, or an error occurs, then
    /// the buffer will be empty and this connection will return to a closed state.
    /// @return A buffer containing data sent from the remote endpoint of this connection.
    /// @param ec An error_code that is set if an error occurs.
    /// @param flags A bitfield of receive_flags constants used to customize this call to
    /// receive().
    buffer receive(
        size_t size, std::error_code &ec, receive_flags_mask flags = receive_flags::none);

    /// @brief Return true if this connection has established a connection with a remote endpoint.
    /// @return True if this connection has established a connection with a remote endpoint.
    bool is_connected() const;

    /// @brief Return the native socket associated with this connection.
    /// @return The native socket associated with this connection.
    socket_type native_socket() const;

    /// @brief Return the local endpoint that this connection is bound to. Invalid if no
    /// connection is established.
    /// @return The local endpoint that this connection is bound to.
    endpoint local_endpoint() const;

    /// @brief Return the remote endpoint that this connection is connected to. Invalid if no
    /// connection is established.
    /// @return The remote endpoint that this connection is connected to.
    endpoint remote_endpoint() const;

   private:
    socket_type socket_ = 0;
    endpoint local_endpoint_;
    endpoint remote_endpoint_;
};

}  // namespace yonaa
