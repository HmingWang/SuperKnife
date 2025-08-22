cmake_minimum_required(VERSION 3.22)
set(name native)
project(${name})

set(CMAKE_CXX_STANDARD 23)
option(WITH_TESTS "编译单元测试案例" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(ansicolor)
include(platform)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/local.cmake)
  include(local)
endif()

include(xcore)

# 添加子目录
if(CMAKE_JS_VERSION)
  message(STATUS "CMake-js编译node addon")
  add_subdirectory(addon)
else()
  message(STATUS "非CMake-js编译，略过node addon")
endif()
