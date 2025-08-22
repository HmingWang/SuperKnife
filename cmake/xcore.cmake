include(FetchContent)

set(XCORE_NAMESPACE "xCore")
set(FETCHCONTENT_QUIET OFF)

# Set CMP0077 to NEW to allow overriding options
cmake_policy(SET CMP0077 NEW)

# Configure xCore options before fetching
set(XCORE_BUILD_TESTS OFF CACHE BOOL "Don't build xCore tests" FORCE)
set(XCORE_INSTALL OFF CACHE BOOL "Don't install xCore" FORCE)
set(BUILD_SHARED_LIBS OFF CACHE BOOL "Build static libraries" FORCE)

# Add the external xCore directory directly
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/external/xcore)
