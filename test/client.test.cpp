#include "yonaa/client.hpp"

#define CATCH_CONFIG_PREFIX_ALL
#include <catch2/catch_test_macros.hpp>

static const std::string hostname("tcpbin.com");
static const std::string service("4242");
static const yonaa::buffer message("Hello!\n");

static yonaa::buffer response_buffer;

void on_data_receive(const yonaa::buffer &data) {
    response_buffer = data;
}

CATCH_TEST_CASE("[yonaa::client] Client test", "[yonaa]") {
    bool connected_to_server      = false;
    bool disconnected_from_server = false;

    yonaa::client client;
    client.set_data_receive_handler(on_data_receive);
    client.set_connect_handler([&]() { connected_to_server = true; });
    client.set_disconnect_handler([&]() { disconnected_from_server = true; });
    client.connect(hostname, service);

    while (!connected_to_server) {}
    CATCH_REQUIRE(client.is_connected());

    client.send_message(message);

    while (!response_buffer) {}
    CATCH_REQUIRE(response_buffer.size() == message.size());

    client.disconnect();
    while (!disconnected_from_server) {}
    CATCH_REQUIRE_FALSE(client.is_connected());
}
