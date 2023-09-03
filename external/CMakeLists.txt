include(FetchContent)

FetchContent_Declare(
  bitmask
  GIT_REPOSITORY https://github.com/oliora/bitmask.git
  GIT_TAG        1.1.2

  PREFIX         bitmask
  SOURCE_DIR     ${PROJECT_SOURCE_DIR}/external/bitmask
)
FetchContent_MakeAvailable(bitmask)

FetchContent_Declare(
  spdlog

  GIT_REPOSITORY https://github.com/gabime/spdlog.git
  GIT_TAG        v1.12.0

  PREFIX         spdlog
  SOURCE_DIR     ${PROJECT_SOURCE_DIR}/external/spdlog
)
FetchContent_MakeAvailable(spdlog)
