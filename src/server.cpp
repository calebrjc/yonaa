#include "yonaa/server.hpp"

#include <spdlog/spdlog.h>

#include "yonaa/addresses.hpp"
#include "yonaa/resolve.hpp"

namespace yonaa {

static client_id next_available_id = 1;

std::ostream &operator<<(std::ostream &os, const client_info &client) {
    return os << fmt::format("client_info(id={}, fd={})", client.id, client.fd);
}

server::server(uint16_t port)
    : port_(port),
      running_(false),
      has_disconnected_clients_(false),
      poll_group_(detail::socket_status::readable) {}

server::~server() {
    if (network_thread_.joinable()) {
        SPDLOG_TRACE("Joining the network thread");
        network_thread_.join();
    }
}

void server::run() {
    if (running_) return;

    // Start network thread
    SPDLOG_TRACE("Spawning network thread");
    network_thread_ = std::thread(&server::network_thread_function_, this);
}

void server::stop() {
    running_ = false;
    SPDLOG_TRACE("Stop signal received");
}

void server::set_data_receive_handler(const data_receive_handler &handler) {
    on_data_receive_ = handler;
}

void server::set_client_connect_handler(const client_connect_handler &handler) {
    on_client_connect_ = handler;
}

void server::set_client_disconnect_handler(const client_disconnect_handler &handler) {
    on_client_disconnect_ = handler;
}

void server::message_client(const buffer &msg, client_id client_id) {
    // Find the client being specified
    client_info *client = client_info_from_id(client_id);
    if (!client || !client->is_connected) return;

    client->conn.send(msg, ec_);

    // If the send fails, assume the client is disconnected
    if (ec_) remove_client(client_id);
}

void server::message_all_clients(const buffer &msg, client_id exclude_client_id) {
    for (const auto &client : clients_) {
        if (client->id == exclude_client_id) continue;

        message_client(msg, client->id);
    }
}

void server::remove_client(client_id client_id) {
    SPDLOG_TRACE("Attempting to mark client {} for removal", client_id);

    client_info *client = client_info_from_id(client_id);
    if (!client) {
        SPDLOG_WARN("Removal failed: could not find information for client {}", client_id);
        return;
    }

    client->is_connected      = false;
    has_disconnected_clients_ = true;
    SPDLOG_DEBUG("Successfully marked client {} for removal", client_id);
}

/// @brief Run the network operations associated with this server.
void server::network_thread_function_() {
    running_ = true;

    // Open the acceptor on the user's port
    resolve_result endpoints = resolve(any_address, std::to_string(port_), ec_);
    if (ec_) {
        SPDLOG_ERROR(
            "Unable to resolve the local address: {}:{}", any_address, std::to_string(port_));
        std::exit(EXIT_FAILURE);
    }

    // TODO(Caleb): Make reuse_address an option in the API
    acceptor_.open(endpoints, ec_, acceptor_config::reuse_address);
    if (ec_) {
        SPDLOG_ERROR("Unable to open an acceptor");
        std::exit(EXIT_FAILURE);
    }

    while (running_) {
        handle_incoming_connections_();
        handle_incoming_messages_();
        handle_disconnected_clients_();

        // TODO(Caleb): Make this sleep configurable
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }

    acceptor_.close();
    SPDLOG_TRACE("Network thread ended");
}

/// @brief Accept all currently pending connections and, add them as clients to the server.
void server::handle_incoming_connections_() {
    while (running_) {
        if (!acceptor_.has_pending_connection()) break;

        client_id new_client_id = next_available_id++;
        SPDLOG_DEBUG("Connection request received. Attempting to create client {}", new_client_id);

        // Create the new client
        auto new_client  = std::make_unique<client_info>();
        new_client->id   = new_client_id;
        new_client->conn = acceptor_.accept(ec_);
        new_client->fd   = new_client->conn.native_socket();

        if (ec_) {
            SPDLOG_WARN("Error accepting a connection; unable to create client {}", new_client_id);
            next_available_id--;

            continue;
        }

        // Add the new client to the server
        clients_.push_back(std::move(new_client));
        clients_.back()->is_connected = true;
        poll_group_.add_socket(clients_.back()->fd);

        // Notify the user that a new client has connected
        SPDLOG_DEBUG("Client {} created: {}", new_client_id, *clients_.back());
        on_client_connect_(new_client_id);
    }

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
    SPDLOG_TRACE("{} active clients", clients_.size());
    if (clients_.size() > 0) {
        for (const auto &client : clients_) { SPDLOG_TRACE("\t{}", *client); }
    }
#endif
}

/// @brief Poll the connections being managed by the server and handle any incoming data.
void server::handle_incoming_messages_() {
    detail::poll_result events = poll_group_.poll();

    for (const auto &[socket_fd, status] : events) {
        if (!running_) return;

        SPDLOG_TRACE("Handling events for fd={}, (status = {})", socket_fd, status.bits());

        // Figure out which client we're processing
        client_info *client = client_info_from_native_socket(socket_fd);
        if (!client) {
            SPDLOG_ERROR("Unable to find client with fd={}", socket_fd);
            continue;
        }

        if (status
            & (yonaa::detail::socket_status::error | yonaa::detail::socket_status::hung_up)) {
            // Assume that the client has disconnected
            SPDLOG_DEBUG("Handling socket error for {}. Marking for removal", *client);
            remove_client(client->id);
            continue;
        }

        if (status & yonaa::detail::socket_status::readable) {
            SPDLOG_DEBUG("Handling readable event for fd={} for client {}", socket_fd, *client);
            buffer data = client->conn.receive(ec_);

            // If the receive failed or no data was received, assume that the client disconnected
            if (ec_ || (data.size() == 0)) {
                SPDLOG_DEBUG("Disconnect message received from {}. Marking for removal", *client);
                remove_client(client->id);
                continue;
            }

            // Notify the user that the client sent some data
            on_data_receive_(data, client->id);
        }
    }
}

/// @brief If present, physically and logically disconnect clients from the server and remove them.
void server::handle_disconnected_clients_() {
    if (!has_disconnected_clients_) return;

    // Gather the "disconnected" clients
    auto first_disconnected_client = std::remove_if(
        clients_.begin(), clients_.end(), [](const auto &client) { return !client->is_connected; });

    // Physically and logically disconnect the clients
    for (auto it = first_disconnected_client; it != clients_.end(); it++) {
        client_info *client = it->get();

        SPDLOG_TRACE("Disconnecting client: {}", *client);
        poll_group_.remove_socket(client->fd);
        client->conn.disconnect();
        on_client_disconnect_(client->id);
    }

    // Remove the disconnected clients from the server
    clients_.erase(first_disconnected_client, clients_.end());
}

/// @brief Return a non-owning pointer to the client_info of the client with the specified id, or
/// nullptr if no such client exists.
/// @param client_id The id of the client to search for.
/// @return A non-owning pointer to the client_info of the client with the specified id, or nullptr
/// if no such client exists.
client_info *server::client_info_from_id(client_id client_id) {
    auto it = std::find_if(
        clients_.begin(), clients_.end(), [&](const auto &it) { return it->id == client_id; });

    // Return the client's info if we could find it, and false otherwise
    return (it != clients_.end()) ? it->get() : nullptr;
}

/// @brief Return a non-owning pointer to the client_info of the client with the specified socket
/// descriptor, or nullptr if no such client exists.
/// @param client_id The id of the client to search for.
/// @return A non-owning pointer to the client_info of the client with the specified socket
/// descriptor, or nullptr if no such client exists.
client_info *server::client_info_from_native_socket(socket_type socket_fd) {
    auto it = std::find_if(
        clients_.begin(), clients_.end(), [&](const auto &it) { return it->fd == socket_fd; });

    // Return the client's info if we could find it, and false otherwise
    return (it != clients_.end()) ? it->get() : nullptr;
}

}  // namespace yonaa
