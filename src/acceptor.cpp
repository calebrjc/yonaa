#include "yonaa/acceptor.hpp"

#include <sys/socket.h>

#include "yonaa/detail/poll.hpp"
#include "yonaa/detail/socket_ops.hpp"

namespace yonaa {

/// @brief The maximum number of connections that an acceptor may queue for accepting.
static const uint64_t backlog_size = 128;

acceptor::acceptor() : socket_(0) {}

acceptor::~acceptor() {
    if (is_open()) close();
}

void acceptor::open(const resolve_result &local_endpoints, acceptor_config_mask cfg) {
    // Delegate function call and throw if necessary
    std::error_code ec;
    open(local_endpoints, ec, cfg);

    if (ec) throw ec;
}

void acceptor::open(
    const resolve_result &local_endpoints, std::error_code &ec, acceptor_config_mask cfg) {
    if (local_endpoints.empty()) {
        // TODO(Caleb): Custom error categories?
        ec.assign(1, std::system_category());
        return;
    }

    bool reuse_addr = (bool)(cfg & acceptor_config::reuse_address);
    socket_type socket_fd =
        detail::socket_ops::create_listening_socket(local_endpoints, backlog_size, reuse_addr);

    if (socket_fd == 0) {
        // TODO(Caleb): Custom error categories?
        ec.assign(32, std::system_category());
        return;
    }

    socket_         = socket_fd;
    local_endpoint_ = detail::socket_ops::get_local_endpoint(socket_fd);
}

void acceptor::close() {
    if (!is_open()) return;

    detail::socket_ops::close_socket(socket_);

    socket_         = 0;
    local_endpoint_ = endpoint();
}

bool acceptor::is_open() const {
    // Note(Caleb): socket_ is only assigned after a connect(), so this is fine.
    return socket_ != 0;
}

bool acceptor::has_pending_connection() const {
    detail::socket_status_mask socket_status = detail::poll_socket(socket_, 0);
    return (socket_status & detail::socket_status::readable) != 0;
}

connection acceptor::accept() const {
    // Delegate function call and throw if necessary
    std::error_code ec;
    auto conn = accept(ec);

    if (ec) throw ec;

    return conn;
}

connection acceptor::accept(std::error_code &ec) const {
    if (!is_open()) {
        // TODO(Caleb): Custom error categories?
        ec.assign(1, std::system_category());
        return connection();
    }

    address_storage_type remote_addr;
    address_size_type remote_addr_size = sizeof(remote_addr);

    int remote_socket_fd = ::accept(socket_, (address_type *)&remote_addr, &remote_addr_size);
    if (remote_socket_fd == -1) {
        // TODO(Caleb): Custom error categories?
        ec.assign(errno, std::system_category());
        return connection();
    }

    auto remote_endpoint = endpoint::from_native_address(
        local_endpoint_.protocol(), (address_type *)&remote_addr, remote_addr_size);
    return connection::from_native_socket(remote_socket_fd, remote_endpoint);
}

endpoint acceptor::local_endpoint() const {
    return local_endpoint_;
}

}  // namespace yonaa
