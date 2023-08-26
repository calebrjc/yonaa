#include "test_utils.hpp"

#include "yonaa/resolve.hpp"
#include "yonaa/detail/socket_ops.hpp"

void simulate_incoming_connection(
    const std::string &hostname, const std::string &service, sim_client_state &scs) {
    // Wait for the server to be ready for an incoming connection
    while (!scs.server_ready) {}

    // Connect to the server
    auto endpoints = yonaa::resolve(hostname, service);
    socket_type socket_fd = yonaa::detail::socket_ops::create_connected_socket(endpoints);
    scs.client_ready = true;

    // Wait for the server to accept the connection
    while (!scs.server_accepted_client) {}

    // Stay open until the server is done with the connection
    while (!scs.server_done) {}

    yonaa::detail::socket_ops::close_socket(socket_fd);
}
