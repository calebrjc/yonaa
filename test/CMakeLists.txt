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
    "acceptor.test.cpp"
    "connection.test.cpp"
    "resolve.test.cpp"
    "detail/poll.test.cpp"
    "detail/socket_ops.test.cpp"
    "test_utils/test_utils.hpp"
    "test_utils/test_utils.cpp")

add_executable(yonaa_tests ${TESTS})
target_link_libraries(yonaa_tests PRIVATE yonaa Catch2::Catch2WithMain pthread)
target_compile_options(yonaa_tests PRIVATE -g -Wall -Wextra --pedantic-errors)

# Automatically register Catch2 tests
list(APPEND CMAKE_MODULE_PATH ${catch2_SOURCE_DIR}/extras)
include(CTest)
include(Catch)
catch_discover_tests(yonaa_tests)
