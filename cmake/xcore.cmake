message(STATUS "Including xCore")

include(FetchContent)

set(XCORE_BUILD_TESTS OFF CACHE BOOL "Don't build xCore tests" FORCE)

set(xCore_DIR ${CMAKE_SOURCE_DIR}/external/xcore)




FetchContent_Declare(
  xCore
  GIT_REPOSITORY git@hming.wang:xcore.git
  GIT_TAG        new
  GIT_SHALLOW    TRUE
  SOURCE_DIR     ${xCore_DIR}
)

FetchContent_MakeAvailable(xCore)
