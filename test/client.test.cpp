#include "yonaa/client.hpp"

#define CATCH_CONFIG_PREFIX_ALL
#include <catch2/catch_test_macros.hpp>

#include "yonaa/addresses.hpp"

static const std::string hostname(yonaa::loopback_address);
static const std::string service("5000");
static const yonaa::buffer message("Hello!\n");

static yonaa::buffer response_buffer;

void on_data_receive(const yonaa::buffer &data) {
    response_buffer = data;
}

CATCH_TEST_CASE("[yonaa::client] Client test", "[yonaa]") {
    bool connected_to_server      = false;
    bool disconnected_from_server = false;

    // Initialize the client
    yonaa::client client;
    client.set_data_receive_handler(on_data_receive);
    client.set_connect_handler([&]() { connected_to_server = true; });
    client.set_disconnect_handler([&]() { disconnected_from_server = true; });

    // Start a thread for the mock server
    auto server_thread = std::thread(
        []() { std::system("rm -f /tmp/f; mkfifo /tmp/f; cat /tmp/f | netcat -l 5000 > /tmp/f"); });

    // Let the mock server start up (I haven't found a way to do this reliably, meaning that the
    // test hangs or fails sometimes)
    std::this_thread::sleep_for(std::chrono::milliseconds(200));

    client.connect(hostname, service);

    // Wait until we've connected to the server
    while (!connected_to_server) {}

    CATCH_REQUIRE(client.is_connected());

    // Send a test message to the server
    client.send_message(message);

    // Wait for the server to respond
    while (!response_buffer) {}

    CATCH_REQUIRE(response_buffer.size() == message.size());

    client.disconnect();

    // Wait for the client to disconnect from the server
    while (!disconnected_from_server) {}
    
    CATCH_REQUIRE_FALSE(client.is_connected());

    if (server_thread.joinable()) server_thread.join();
}
