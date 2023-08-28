#pragma once

#include <functional>
#include <system_error>
#include <thread>

#include "yonaa/buffer.hpp"
#include "yonaa/connection.hpp"

namespace yonaa {

class client final {
   public:
    using data_receive_handler = std::function<void(const buffer &)>;
    using connect_handler      = std::function<void()>;
    using disconnect_handler   = std::function<void()>;

   public:
    /// @brief Create a client.
    client() = default;

    /// @brief Disconnect this client.
    ~client();

    // Disable copies and moves --------------------------------------------------------------------

    client(const client &other)             = delete;
    client &operator=(const client &other)  = delete;
    client(const client &&other)            = delete;
    client &operator=(const client &&other) = delete;

    // ---------------------------------------------------------------------------------------------

    /// @brief Install a function for this client to call when it receives data from the server.
    /// @param handler The function to be called.
    void set_connect_handler(const connect_handler &handler);

    /// @brief Install a function for this client to call when it connects to a server.
    /// @param handler The function to be called.
    void set_disconnect_handler(const disconnect_handler &handler);

    /// @brief Install a function for this client to call when it disconnects from a server.
    /// @param handler The function to be called.
    void set_data_receive_handler(const data_receive_handler &handler);

    /// @brief Starts theh client's network thread, in which a connection to the supplied hostname
    /// and service pair is initiated and monitored.
    /// @param hostname The name of the host to connect to.
    /// @param service The service on which to connect to the host.
    void connect(const std::string &hostname, const std::string &service);

    /// @brief Request the the client disconnect from the server and stop its network thread.
    void disconnect();

    /// @brief Send data to the server.
    /// @param msg The data to be sent.
    void send_message(const buffer &msg);

    /// @brief Return false if the network thread is joined (or attempting to), and true otherwise.
    /// @return False if the network thread is joined (or attempting to), and true otherwise.
    bool is_running() const { return running_; }

    /// @brief Return true if the client is currently connected to the server, and false otherwise.
    /// @return True if the client is currently connected to the server, and false otherwise.
    bool is_connected() const;

   private:
    void network_thread_function_();
    void handle_incoming_messages_();

   private:
    bool running_;
    std::error_code ec_;

    data_receive_handler on_data_receive_;
    connect_handler on_connect_;
    disconnect_handler on_disconnect_;

    // TODO(Caleb): Add an error handling callback function

    std::thread network_thread_;
    connection conn_;

    struct {
        std::string hostname;
        std::string service;
    } server_addr_;
};

}  // namespace yonaa
