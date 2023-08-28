#include "yonaa/connection.hpp"

#include "yonaa/detail/poll.hpp"
#include "yonaa/detail/socket_ops.hpp"

namespace yonaa {

/// @brief The maximum size of a buffer to be used for message transceiving.
static const size_t max_buffer_size = 8192;

connection::connection() : socket_(0) {}

connection connection::from_native_socket(socket_type socket_fd, const endpoint &remote_endpoint) {
    connection conn;
    conn.socket_          = socket_fd;
    conn.local_endpoint_  = detail::socket_ops::get_local_endpoint(socket_fd);
    conn.remote_endpoint_ = remote_endpoint;

    return conn;
}

connection::~connection() {
    if (is_connected()) disconnect();
}

connection::connection(connection &&other) {
    *this = std::move(other);
}

connection &connection::operator=(connection &&other) {
    socket_          = other.socket_;
    local_endpoint_  = other.local_endpoint_;
    remote_endpoint_ = other.remote_endpoint_;

    other.socket_          = 0;
    other.local_endpoint_  = endpoint();
    other.remote_endpoint_ = endpoint();

    return *this;
}

void connection::connect(const resolve_result &remote_endpoints) {
    // Delegate function call and throw if necessary
    std::error_code ec;
    connect(remote_endpoints, ec);

    if (ec) throw ec;
}

void connection::connect(const resolve_result &remote_endpoints, std::error_code &ec) {
    if (remote_endpoints.empty()) {
        // TODO(Caleb): Custom error categories?
        ec.assign(1, std::system_category());
        return;
    }

    // Attempt to create a connected socket
    int socket_fd = detail::socket_ops::create_connected_socket(remote_endpoints);

    if (socket_fd == 0) {
        // TODO(Caleb): Custom error categories?
        ec.assign(errno, std::system_category());
        return;
    }

    socket_          = socket_fd;
    local_endpoint_  = detail::socket_ops::get_local_endpoint(socket_);
    remote_endpoint_ = detail::socket_ops::get_remote_endpoint(socket_);
}

void connection::disconnect() {
    if (!is_connected()) return;

    detail::socket_ops::close_socket(socket_);

    socket_          = 0;
    local_endpoint_  = endpoint();
    remote_endpoint_ = endpoint();
}

void connection::send(const buffer &data, send_flags_mask flags) const {
    // Delegate function call and throw if necessary
    std::error_code ec;
    send(data, ec, flags);

    if (ec) throw ec;
}

void connection::send(const buffer &data, std::error_code &ec, send_flags_mask flags) const {
    if (!is_connected() || data.size() == 0) {
        ec.assign(1, std::system_category());
        return;
    }

    const char *send_buffer = data.data();
    size_t send_buffer_size = data.size();

    // Translate flags
    int send_flags = 0;
    send_flags |= (flags & send_flags::dont_route) ? MSG_DONTROUTE : 0;
    send_flags |= (flags & send_flags::end_of_record) ? MSG_EOR : 0;

    size_t bytes_sent = 0;
    while (bytes_sent < send_buffer_size) {
        int send_result =
            ::send(socket_, send_buffer + bytes_sent, send_buffer_size - bytes_sent, send_flags);

        if (send_result == -1) {
            // TODO(Caleb): Custom error categories?
            ec.assign(errno, std::system_category());
            return;
        }
        bytes_sent += send_result;
    }
}

buffer connection::receive(receive_flags_mask flags) {
    // Delegate function call and throw if necessary
    std::error_code ec;
    auto data = receive(max_buffer_size, ec, flags);

    if (ec) throw ec;

    return data;
}

buffer connection::receive(size_t size, receive_flags_mask flags) {
    // Delegate function call and throw if necessary
    std::error_code ec;
    auto data = receive(size, ec, flags);

    if (ec) throw ec;

    return data;
}

buffer connection::receive(std::error_code &ec, receive_flags_mask flags) {
    // Delegate function call and throw if necessary
    auto data = receive(max_buffer_size, ec, flags);

    return data;
}

buffer connection::receive(size_t size, std::error_code &ec, receive_flags_mask flags) {
    if (!is_connected() || size == 0) {
        ec.assign(1, std::system_category());
        return buffer();
    }

    buffer receive_buffer(size);

    // Translate flags
    int recv_flags = 0;
    recv_flags |= (flags & receive_flags::peek) ? MSG_PEEK : 0;

    int recv_result = ::recv(socket_, receive_buffer.data(), size, recv_flags);

    if (recv_result == 0) {  // The remote endpoint is disconnected.
        disconnect();

        // TODO(Caleb): Custom error categories?
        ec.assign(errno, std::system_category());
        return buffer();
    }

    if (recv_result == -1) {
        // TODO(Caleb): Custom error categories?
        ec.assign(errno, std::system_category());
        return buffer();
    }

    receive_buffer.resize(recv_result);
    return receive_buffer;
}

bool connection::is_connected() const {
    // Note(Caleb): socket_ is only assigned after a connect(), so this is fine.
    return socket_ != 0;
}

bool connection::has_data_available() const {
    auto status = detail::poll_socket(socket_);
    return (bool)(status & detail::socket_status::readable);
}

socket_type connection::native_socket() const {
    return socket_;
}

endpoint connection::local_endpoint() const {
    return local_endpoint_;
}
endpoint connection::remote_endpoint() const {
    return remote_endpoint_;
}

}  // namespace yonaa
