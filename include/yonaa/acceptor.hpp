#pragma once

#include <system_error>

#include "bitmask/bitmask.hpp"
#include "yonaa/connection.hpp"
#include "yonaa/resolve.hpp"
#include "yonaa/types.hpp"

namespace yonaa {

/// @brief Flags used to customize the behavior of an open acceptor.
enum class acceptor_config {
    none          = 0x00,
    reuse_address = 0x01,  /// @brief Specifies that this acceptor should use the SO_REUSEADDR
                           /// option when configuring its socket. (see: man 7 ip)
};
BITMASK_DEFINE_MAX_ELEMENT(acceptor_config, reuse_address)
using acceptor_config_mask = bitmask::bitmask<acceptor_config>;

/// @brief A networking entity that allows a host to listen for and accept incoming connections.
class acceptor {
   public:
    /// @brief Create an unopened and unbound acceptor.
    acceptor();

    /// @brief Cleanup after an acceptor.
    ~acceptor();

    // Disable copies and moves --------------------------------------------------------------------

    acceptor(const acceptor &other)             = delete;
    acceptor &operator=(const acceptor &other)  = delete;
    acceptor(const acceptor &&other)            = delete;
    acceptor &operator=(const acceptor &&other) = delete;

    // ---------------------------------------------------------------------------------------------

   public:
    /// @brief Bind this acceptor to the resolved local address and open it to incoming connections.
    /// @param local_endpoints The resolved local address.
    /// @param cfg A bitmask of acceptor_config values to customize how this acceptor is opened.
    void open(
        const resolve_result &local_endpoints, acceptor_config_mask cfg = acceptor_config::none);

    /// @brief Bind this acceptor to the resolved local address and open it to incoming connections.
    /// @param local_endpoints The resolved local address.
    /// @param ec An error_code that is set if an error occurs.
    /// @param cfg A bitmask of acceptor_config values to customize how this acceptor is opened.
    void open(
        const resolve_result &local_endpoints,
        std::error_code &ec,
        acceptor_config_mask cfg = acceptor_config::none);

    /// @brief Stop this acceptor from accepting incoming connections and close it.
    void close();

    /// @brief Return true if this acceptor is open to incoming connections.
    /// @return True if this acceptor is open to incoming connections.
    bool is_open() const;

    /// @brief Return true if this acceptor has a connection waiting to be accepted.
    /// @return True if this acceptor has a connection waiting to be accepted.
    bool has_pending_connection() const;

    /// @brief Return a connection to the remote attempting to connect to this acceptor.
    /// @return A connection to the remote attempting to connect to this acceptor.
    connection accept() const;

    /// @brief Return a connection to the remote attempting to connect to this acceptor.
    /// @return A connection to the remote attempting to connect to this acceptor.
    /// @param ec An error_code that is set if an error occurs.
    connection accept(std::error_code &ec) const;

    /// @brief Return the local endpoint that this acceptor is bound to. Invalid if this acceptor is
    /// not bound.
    /// @return The local endpoint that this acceptor is bound to.
    endpoint local_endpoint() const;

   private:
    socket_type socket_;
    endpoint local_endpoint_;
};

}  // namespace yonaa
