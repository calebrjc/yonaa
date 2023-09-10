#include "yonaa/detail/poll.hpp"

#include <poll.h>

namespace yonaa::detail {

namespace detail {

/// @brief Return a socket_status_mask that represents the status described by revents.
/// @param revents The events to convert.
/// @return A socket_status_mask that represents the status described by revents.
socket_status_mask ssm_from_revents(int revents) {
    socket_status_mask ssm;

    ssm |= (revents & POLLIN) ? socket_status::readable : socket_status::none;
    ssm |= (revents & POLLOUT) ? socket_status::writable : socket_status::none;
    ssm |= (revents & POLLERR) ? socket_status::error : socket_status::none;
    ssm |= (revents & POLLHUP) ? socket_status::hung_up : socket_status::none;

    return ssm;
}

}  // namespace detail

socket_status_mask poll_socket(socket_type socket_fd, int timeout_millis) {
    pollfd pfd = {0, 0, 0};
    pfd.fd     = socket_fd;
    pfd.events = POLLIN | POLLOUT;

    int num_events = ::poll(&pfd, 1, timeout_millis);

    return (num_events > 0) ? detail::ssm_from_revents(pfd.revents) : socket_status::none;
}

poll_group::poll_group(socket_status_mask config) : pfd_config_(0) {
    pfd_config_ |= (config & socket_status::readable) ? POLLIN : 0;
    pfd_config_ |= (config & socket_status::writable) ? POLLOUT : 0;
}

void poll_group::add_socket(socket_type socket_fd) {
    pollfd pfd = {0, 0, 0};
    pfd.fd     = socket_fd;
    pfd.events = pfd_config_;

    pfds_.push_back(pfd);
}

void poll_group::remove_socket(socket_type socket_fd) {
    auto is_target_pfd = [&](pollfd pfd) { return pfd.fd == socket_fd; };
    pfds_.erase(std::remove_if(pfds_.begin(), pfds_.end(), is_target_pfd), pfds_.end());
}

poll_result poll_group::poll(int timeout_millis) {
    (void)::poll(pfds_.data(), pfds_.size(), timeout_millis);

    poll_result result;
    for (const pollfd &pfd : pfds_) {
        if (!pfd.revents) continue;

        result.push_back({pfd.fd, detail::ssm_from_revents(pfd.revents)});
    }

    return result;
}

size_t poll_group::size() const {
    return pfds_.size();
}

}  // namespace yonaa::detail::poll
