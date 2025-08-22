include(FetchContent)

set(XCORE_NAMESPACE "xCore")
set(FETCHCONTENT_QUIET OFF)

FetchContent_Declare(
    xcore
    GIT_REPOSITORY git@hming.wang:xcore.git
    GIT_TAG new
    GIT_SHALLOW 1
    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/xcore
)

# Set CMP0077 to NEW to allow overriding options
cmake_policy(SET CMP0077 NEW)

# Disable tests and installation in external projects
set(XCORE_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(XCORE_INSTALL OFF CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(xcore)
