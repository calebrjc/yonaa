#include "yonaa/detail/poll.hpp"

#include <thread>

#define CATCH_CONFIG_PREFIX_ALL
#include <catch2/catch_test_macros.hpp>

#include "../test_utils/test_utils.hpp"
#include "yonaa/addresses.hpp"
#include "yonaa/connection.hpp"
#include "yonaa/detail/socket_ops.hpp"

static const std::string remote_hostname("tcpbin.com");
static const std::string remote_service("4242");
static const yonaa::resolve_result remote_endpoint =
    yonaa::resolve(remote_hostname, remote_service);

static const std::string local_hostname = yonaa::loopback_address;
static const std::string local_service("5000");
static const yonaa::resolve_result local_endpoint = yonaa::resolve(local_hostname, local_service);

CATCH_TEST_CASE("[yonaa::detail::poll] poll_socket()", "[net]") {
    std::error_code ec;
    yonaa::resolve_result endpoints;
    socket_type socket_fd;

    CATCH_SECTION("Connected socket test") {
        // Create the connected socket
        socket_fd = yonaa::detail::socket_ops::create_connected_socket(remote_endpoint);

        auto status = yonaa::detail::poll_socket(socket_fd);

        // The socket should be writable...
        CATCH_REQUIRE(status & yonaa::detail::socket_status::writable);

        // ... and not have any waiting data to read...
        CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::readable);

        // ... and not be in an error state...
        CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::error);

        // .. and be currently connected.
        CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::hung_up);

        yonaa::detail::socket_ops::close_socket(socket_fd);
    }

    CATCH_SECTION("Listening socket test") {
        // Start sim client thread
        sim_client_state scs;
        auto client_thread =
            std::thread([&] { simulate_incoming_connection(local_hostname, local_service, scs); });

        // Set up acceptor socket
        socket_fd = yonaa::detail::socket_ops::create_listening_socket(local_endpoint, 128, true);
        scs.server_ready = true;

        while (!scs.client_ready) {}  // Wait for the client to attempt to connect
        auto status = yonaa::detail::poll_socket(socket_fd);
        scs.server_accepted_client = true;

        // The socket should not be writable...
        CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::writable);

        // ... and have a waiting connection (be readable)
        CATCH_REQUIRE(status & yonaa::detail::socket_status::readable);

        // ... and not be in an error state...
        CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::error);

        // .. and be currently connected.
        CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::hung_up);

        scs.server_done = true;
        if (client_thread.joinable()) client_thread.join();
    }
}

CATCH_TEST_CASE("[yonaa::detail::poll] poll_group", "[net]") {
    yonaa::detail::poll_group pg;
    std::error_code ec;

    yonaa::connection connection_1;
    yonaa::connection connection_2;

    // Initial state -------------------------------------------------------------------------------
    CATCH_REQUIRE(pg.size() == 0);
    // ---------------------------------------------------------------------------------------------

    connection_1.connect(remote_endpoint, ec);
    CATCH_REQUIRE_FALSE(ec);

    connection_2.connect(remote_endpoint, ec);
    CATCH_REQUIRE_FALSE(ec);

    // Adding sockets increases size ---------------------------------------------------------------
    pg.add_socket(connection_1.native_socket());
    pg.add_socket(connection_2.native_socket());

    CATCH_REQUIRE(pg.size() == 2);
    // ---------------------------------------------------------------------------------------------

    connection_1.send(yonaa::buffer("Hello!\n"), ec);
    CATCH_REQUIRE_FALSE(ec);

    // Wait until the connection has has data waiting to be read (archaic workaround method)
    while ((connection_1.receive(ec, yonaa::receive_flags::peek)).is_empty()) {}

    // poll() returns expected results -------------------------------------------------------------
    auto pr = pg.poll();
    CATCH_REQUIRE(pr.size() == 2);

    for (const auto &[fd, status] : pr) {
        if (fd == connection_1.native_socket()) {
            // Should have data waiting to recv
            CATCH_REQUIRE(status & yonaa::detail::socket_status::readable);

            // Should be able to send data
            CATCH_REQUIRE(status & yonaa::detail::socket_status::writable);

            // No error or hang up
            CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::error);
            CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::hung_up);

            continue;
        }

        if (fd == connection_2.native_socket()) {
            // Should not have data waiting to recv
            CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::readable);

            // Should be able to send data
            CATCH_REQUIRE(status & yonaa::detail::socket_status::writable);

            // No error or hang ups
            CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::error);
            CATCH_REQUIRE_FALSE(status & yonaa::detail::socket_status::hung_up);

            continue;
        }

        CATCH_REQUIRE(0);
    }
    //----------------------------------------------------------------------------------------------

    // Removing sockets decreases size -------------------------------------------------------------
    pg.remove_socket(connection_1.native_socket());
    pg.remove_socket(connection_2.native_socket());

    CATCH_REQUIRE(pg.size() == 0);
    // ---------------------------------------------------------------------------------------------
}
