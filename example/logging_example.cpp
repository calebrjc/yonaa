#include <yonaa/logging.hpp>

int main(int argc, char **argv) {
    (void)argc;
    (void)argv;

    yonaa::logging::init("logging_example");

    YONAA_TRACE("Hello, World!");
    YONAA_DEBUG("Hello, World!");
    YONAA_INFO("Hello, World!");
    YONAA_WARN("Hello, World!");
    YONAA_ERROR("Hello, World!");
    YONAA_CRITICAL("Hello, World!");

    return 0;
}
