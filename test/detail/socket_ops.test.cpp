#include "yonaa/detail/socket_ops.hpp"

#define CATCH_CONFIG_PREFIX_ALL
#include <catch2/catch_test_macros.hpp>

#include "yonaa/addresses.hpp"
#include "yonaa/resolve.hpp"

CATCH_TEST_CASE("[yonaa::detail::socket_ops] create_listening_socket()") {
    std::string hostname = yonaa::any_address;
    std::string service("5000");

    auto rr               = yonaa::resolve(hostname, service);
    socket_type socket_fd = yonaa::detail::socket_ops::create_listening_socket(rr, 128, true);

    CATCH_REQUIRE(socket_fd != 0);
}
