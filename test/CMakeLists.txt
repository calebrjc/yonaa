include(FetchContent)

# Add Catch2
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG        v3.4.0
)
FetchContent_MakeAvailable(Catch2)

# Create tests target
set(TESTS
    "${CMAKE_CURRENT_SOURCE_DIR}/acceptor.test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/client.test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/connection.test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/resolve.test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/detail/poll.test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/detail/socket_ops.test.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_utils/test_utils.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/test_utils/test_utils.cpp")

add_executable(yonaa_tests ${TESTS})
target_link_libraries(yonaa_tests PRIVATE yonaa Catch2::Catch2WithMain)
target_compile_options(yonaa_tests PRIVATE -g -Wall -Wextra --pedantic-errors)

# Automatically register Catch2 tests
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(yonaa_tests)
