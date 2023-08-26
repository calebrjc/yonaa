#pragma once

#include <string>

/// @brief A structure used to communicate between a simulated client and simulated server about the
/// state of the shared connection.
struct sim_client_state {
    // True when the server is ready for the client to attempt to connect
    bool server_ready = false;

    // True when the client is ready to have its connection accepted
    bool client_ready = false;

    // True when the server has accepted the client connection
    bool server_accepted_client = false;

    // True when the server is finished with the client
    bool server_done = false;
};

void simulate_incoming_connection(
    const std::string &hostname, const std::string &service, sim_client_state &scs);
