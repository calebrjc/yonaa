#pragma once

#include <poll.h>

#include <vector>

#include "bitmask/bitmask.hpp"
#include "yonaa/types.hpp"

namespace yonaa::detail {

/// @brief Flags used to represent the current status of a socket.
enum class socket_status {
    none     = 0x00,
    error    = 0x01,
    hung_up  = 0x02,
    readable = 0x04,
    writable = 0x08,
};
// Note(Caleb): Not sure why cppcheck emits this error, but everything's fine, I swear
// cppcheck-suppress returnDanglingLifetime
BITMASK_DEFINE_MAX_ELEMENT(socket_status, writable)
using socket_status_mask = bitmask::bitmask<socket_status>;

/// @brief Return the status of a particular socket.
/// @param socket_fd The socket to be polled.
/// @param timeout_millis The timeout, in milliseconds, to wait for a state change if the socket
/// does not have one. A negative timeout will cause this function to block until a state change
/// occurs.
/// @return The status of a particular socket.
socket_status_mask poll_socket(socket_type socket_fd, int timeout_millis = 0);

/// @brief A data structure to relate socket file descriptors to socket statuses.
struct socket_status_info {
    socket_type socket_fd;
    socket_status_mask status;
};

// A data structure used to convey the result of polling multiple sockets.
using poll_result = std::vector<socket_status_info>;

/// @brief A utility for polling multiple sockets.
class poll_group {
   public:
    poll_group(socket_status_mask config = socket_status::readable | socket_status::writable);

    // Disable copies and moves --------------------------------------------------------------------

    poll_group(const poll_group &other)             = delete;
    poll_group &operator=(const poll_group &other)  = delete;
    poll_group(const poll_group &&other)            = delete;
    poll_group &operator=(const poll_group &&other) = delete;

    // ---------------------------------------------------------------------------------------------

    /// @brief Add a socket to this poll group.
    /// @param socket_fd The socket to be added.
    void add_socket(socket_type socket_fd);

    /// @brief Remove a socket from this poll group.
    /// @param socket_fd The socket to be removed.
    void remove_socket(socket_type socket_fd);

    /// @brief Return the status of all sockets in this poll group that experienced status changes.
    /// @param timeout_millis The timeout, in milliseconds, to wait for a state change if the socket
    /// does not have one. A negative timeout will cause this function to block until a state change
    /// occurs.
    /// @return The status of all sockets in this poll group that experienced status changes.
    poll_result poll(int timeout_millis = 0);

    /// @brief Return the number of sockets associated with this poll_group.
    /// @return The number of sockets associated with this poll_group.
    size_t size() const;

   private:
    std::vector<pollfd> pfds_;
    int pfd_config_;
};

}  // namespace yonaa::detail
