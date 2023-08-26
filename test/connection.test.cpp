#include "yonaa/connection.hpp"

#define CATCH_CONFIG_PREFIX_ALL
#include <catch2/catch_test_macros.hpp>

static const std::string hostname("tcpbin.com");
static const std::string service("4242");
static const yonaa::buffer message("Hello!\n");

CATCH_TEST_CASE("[yonaa::connection] Initial state is correct", "[net]") {
    yonaa::connection conn;
    std::error_code ec;
    
    // The connection should not be connected...
    CATCH_REQUIRE_FALSE(conn.is_connected());

    // ... and should fail to send data...
    conn.send(message, ec);
    CATCH_REQUIRE(ec);

    // ... and should fail to receive data...
    auto buffer = conn.receive(ec);
    CATCH_REQUIRE(ec);

    // ... and should not be bound to any valid local endpoint...
    CATCH_REQUIRE(conn.local_endpoint() == yonaa::endpoint());

    // ... and should not be connected to any valid remote endpoint...
    CATCH_REQUIRE(conn.remote_endpoint() == yonaa::endpoint());
}

CATCH_TEST_CASE("[yonaa::connection] Connection cannot be opened with invalid endpoints", "[net]") {
    yonaa::connection conn;
    std::error_code ec;

    conn.connect(yonaa::resolve_result(), ec);

    // An error should have been reported...
    CATCH_REQUIRE(ec);
    
    // The connection should not be connected...
    CATCH_REQUIRE_FALSE(conn.is_connected());

    // ... and should fail to send data...
    conn.send(message, ec);
    CATCH_REQUIRE(ec);

    // ... and should fail to receive data...
    auto buffer = conn.receive(ec);
    CATCH_REQUIRE(ec);

    // ... and should not be bound to any valid local endpoint...
    CATCH_REQUIRE(conn.local_endpoint() == yonaa::endpoint());

    // ... and should not be connected to any valid remote endpoint...
    CATCH_REQUIRE(conn.remote_endpoint() == yonaa::endpoint());
}

CATCH_TEST_CASE("[yonaa::connection] Connection can be opened with valid endpoints", "[net]") {
    yonaa::connection conn;
    std::error_code ec;

    auto endpoints = yonaa::resolve(hostname, service);
    conn.connect(endpoints, ec);

    // An error should have been reported...
    CATCH_REQUIRE_FALSE(ec);
    
    // The connection should not be connected...
    CATCH_REQUIRE(conn.is_connected());

    // ... and should not be bound to any valid local endpoint...
    CATCH_REQUIRE_FALSE(conn.local_endpoint() == yonaa::endpoint());

    // ... and should not be connected to any valid remote endpoint...
    CATCH_REQUIRE_FALSE(conn.remote_endpoint() == yonaa::endpoint());

    CATCH_SECTION("Functional test") {
        conn.send(message, ec);
        CATCH_REQUIRE_FALSE(ec);

        auto buffer = conn.receive(ec);
        CATCH_REQUIRE_FALSE(ec);
        CATCH_REQUIRE(buffer.size() > 0);
    }
}
