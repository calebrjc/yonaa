#pragma once

#include <functional>
#include <map>
#include <memory>
#include <thread>

#include "yonaa/acceptor.hpp"
#include "yonaa/buffer.hpp"
#include "yonaa/connection.hpp"
#include "yonaa/detail/poll.hpp"

namespace yonaa {

/// @brief Represents an identification number for a client.
using client_id = uint64_t;

/// @brief A struct that describes a client.
struct client_info {
    client_id id;
    connection conn;
    socket_type fd;
    bool is_connected = false;
};

/// @brief Return os with a string representation of client inserted.
/// @param os The stream to insert the string representation into.
/// @param client The client to stringify.
/// @return os with a string representation of client inserted.
std::ostream &operator<<(std::ostream &os, const client_info &client);

class server final {
   public:
    /// @brief The signature for a callback function supplied to the server to be called when
    /// incoming data is received from a particular client.
    using data_receive_handler = std::function<void(const buffer &, client_id)>;

    /// @brief The signature for a callback function supplied to the server to be called when a
    /// client connects to the server.
    using client_connect_handler = std::function<void(client_id)>;

    /// @brief The signature for a callback function supplied to the server to be called when a
    /// client disconnects from the server.
    using client_disconnect_handler = std::function<void(client_id)>;

   public:
    /// @brief Create a server that will listen for incoming connections on the given port.
    /// @param port The port to listen for incoming connections on.
    explicit server(uint16_t port);

    /// @brief Close this server.
    ~server();

    // Disable copies and moves --------------------------------------------------------------------

    server(const server &other)             = delete;
    server &operator=(const server &other)  = delete;
    server(const server &&other)            = delete;
    server &operator=(const server &&other) = delete;

    // ---------------------------------------------------------------------------------------------

    /// @brief Start the server's network thread.
    void run();

    /// @brief Request that the server stop its operation and join the network thread.
    void stop();

    /// @brief Install a function for this server to call when it receives data from a client.
    /// @param handler The function to be called.
    void set_data_receive_handler(const data_receive_handler &handler);

    /// @brief Install a function for this server to call when a client connects to it.
    /// @param handler The function to be called.
    void set_client_connect_handler(const client_connect_handler &handler);

    /// @brief Install a function for this server to call when a client disconnects from it.
    /// @param handler The function to be called.
    void set_client_disconnect_handler(const client_disconnect_handler &handler);

    /// @brief Send data to a client.
    /// @param msg The data to be sent.
    /// @param client_id The id of the client to receive the message.
    void message_client(const buffer &msg, client_id client_id);

    /// @brief Send data to all but (optionally) a single client.
    /// @param msg The data to be sent.
    /// @param exclude_client_id If specified, the id of the client that this data should not be
    /// sent to.
    void message_all_clients(const buffer &msg, client_id exclude_client_id = 0);

    /// @brief Mark a client for disconnection and removal. (see: "kick", "boot", "kill")
    /// @param client_id The id of the client to be disconnected and removed.
    void remove_client(client_id client_id);

    /// @brief Return false if the network thread is joined (or attempting to), and true otherwise.
    /// @return False if the network thread is joined (or attempting to), and true otherwise.
    bool is_running() { return running_; };

   private:
    void network_thread_function_();
    void handle_incoming_connections_();
    void handle_incoming_messages_();
    void handle_disconnected_clients_();
    client_info *client_info_from_id(client_id client_id);
    client_info *client_info_from_native_socket(socket_type socket_fd);

   private:
    uint16_t port_;
    bool running_;
    bool has_disconnected_clients_;
    std::error_code ec_;
    std::vector<std::unique_ptr<client_info>> clients_;

    data_receive_handler on_data_receive_;
    client_connect_handler on_client_connect_;
    client_disconnect_handler on_client_disconnect_;

    std::thread network_thread_;
    acceptor acceptor_;
    detail::poll_group poll_group_;
};

}  // namespace yonaa
