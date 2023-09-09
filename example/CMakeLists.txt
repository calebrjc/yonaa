add_executable(logging_example logging_example.cpp)
target_link_libraries(logging_example PRIVATE yonaa)
target_compile_options(logging_example PRIVATE -g -Wall -Wextra --pedantic-errors)
