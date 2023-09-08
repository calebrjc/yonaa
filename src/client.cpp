#include "yonaa/client.hpp"

#include "yonaa/logging.hpp"
#include "yonaa/resolve.hpp"

namespace yonaa {
client::~client() {
    if (network_thread_.joinable()) {
        YONAA_INTERNAL_TRACE("Joining the network thread");
        network_thread_.join();
    }
}

void client::set_connect_handler(const connect_handler &handler) {
    on_connect_ = handler;
}

void client::set_disconnect_handler(const disconnect_handler &handler) {
    on_disconnect_ = handler;
}

void client::set_data_receive_handler(const data_receive_handler &handler) {
    on_data_receive_ = handler;
}

void client::connect(const std::string &hostname, const std::string &service) {
    server_addr_ = {hostname, service};

    YONAA_INTERNAL_TRACE("Spawning network thread");
    network_thread_ = std::thread(&client::network_thread_function_, this);
}

void client::disconnect() {
    running_ = false;
    YONAA_INTERNAL_TRACE("Stop signal received");
}

void client::send_message(const buffer &msg) {
    conn_.send(msg, ec_);

    // If the send fails, assume we have been disconnected
    if (ec_) { on_disconnect_(); }
}

bool client::is_connected() const {
    return conn_.is_connected();
}

void client::network_thread_function_() {
    YONAA_INTERNAL_TRACE("Network thread started");

    auto remote_endpoints = resolve(server_addr_.hostname, server_addr_.service, ec_);
    if (ec_) {
        YONAA_INTERNAL_ERROR(
            "Unable to resolve address: {}:{}", server_addr_.hostname, server_addr_.service);
        std::exit(EXIT_FAILURE);
    }

    conn_.connect(remote_endpoints, ec_);
    if (ec_) {
        YONAA_INTERNAL_ERROR("Unable to open a connection to the remote");
        std::exit(EXIT_FAILURE);
    }
    on_connect_();

    while (running_) {
        handle_incoming_messages_();

        // TODO(Caleb): Make this sleep configurable
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    conn_.disconnect();
    on_disconnect_();

    YONAA_INTERNAL_TRACE("Network thread ended");
}

void client::handle_incoming_messages_() {
    if (!conn_.is_connected()) {
        on_disconnect_();
        return;
    }

    if (conn_.has_data_available()) { return; }

    buffer data = conn_.receive(ec_);

    if (ec_ || (data.size() == 0)) {
        YONAA_INTERNAL_DEBUG("Disconnect message received");
        on_disconnect_();
        return;
    }

    on_data_receive_(data);
}

}  // namespace yonaa
