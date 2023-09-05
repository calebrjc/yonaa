cmake_minimum_required(VERSION 3.16)

project(
    yonaa
    DESCRIPTION "YOu're Not Alone Anymore! An easy-to-use C++ networking library."
    LANGUAGES C CXX
)

# Build options
option(YONAA_BUILD_TESTS "Build tests" OFF)

# Ensure -std=c++xx instead of -std=g++xx
set(CMAKE_CXX_EXTENSIONS OFF)

# Ensure folder support in IDEs
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Add external libs and dependencies
add_subdirectory(external)
find_package(Threads REQUIRED)

# Add files
set(YONAA_HEADERS
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/yonaa.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/acceptor.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/addresses.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/buffer.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/client.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/connection.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/endpoint.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/resolve.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/server.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/types.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/detail/getaddrinfo.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/detail/poll.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/detail/sockaddr_ops.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/yonaa/detail/socket_ops.hpp")

set(YONAA_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/src/acceptor.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/client.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/connection.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/endpoint.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/resolve.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/server.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detail/getaddrinfo.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detail/poll.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detail/sockaddr_ops.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/detail/socket_ops.cpp")

# Create library
add_library(yonaa ${YONAA_HEADERS} ${YONAA_SOURCES})

target_include_directories(yonaa PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_link_libraries(yonaa
    PUBLIC bitmask spdlog Threads::Threads)

target_compile_features(yonaa PUBLIC cxx_std_17)

target_compile_options(yonaa PRIVATE -Wall -Wextra --pedantic-errors)

# Conditionally enable tests
if(YONAA_BUILD_TESTS)
    add_subdirectory(test)
    enable_testing()
endif()
