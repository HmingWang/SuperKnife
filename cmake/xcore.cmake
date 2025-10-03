message(STATUS "Including xCore")

include(FetchContent)

set(XCORE_BUILD_TESTS OFF CACHE BOOL "Don't build xCore tests" FORCE)


FetchContent_Declare(
  xCore
  GIT_REPOSITORY git@hming.wang:xcore.git
  GIT_TAG        new
)

FetchContent_MakeAvailable(xCore)
