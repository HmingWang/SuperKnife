cmake_minimum_required(VERSION 3.22)
set(name native)
project(${name})

set(CMAKE_CXX_STANDARD 23)
set(WITH_TESTS true)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 添加子目录
add_subdirectory(${name})

# 查找 Node.js 和 Electron 头文件
execute_process(COMMAND node -p "require('node-addon-api').include"
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND node -p "process.execPath.replace(/bin.*$/, 'include/node')"
  OUTPUT_VARIABLE NODE_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE)

message(STATUS "Node.js include dir: ${NODE_INCLUDE_DIR}")
message(STATUS "Node-addon-api include dir: ${NODE_ADDON_API_DIR}")

include_directories(${NODE_INCLUDE_DIR} ${NODE_ADDON_API_DIR})

# #############|test|################
if(WITH_TESTS)
  include(CTest)
  include(catch2)
  add_subdirectory(tests)
endif()
