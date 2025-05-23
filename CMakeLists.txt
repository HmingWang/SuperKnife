cmake_minimum_required(VERSION 3.22)
set(name native)
project(${name})

set(CMAKE_CXX_STANDARD 23)
set(WITH_TESTS true)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# 1. 查找 Node.js 可执行文件路径
execute_process(
  COMMAND node -p "require('node-addon-api').include"
  OUTPUT_VARIABLE NODE_ADDON_API_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 使用正则表达式去除引号
string(REGEX REPLACE "[\r\n\"]" "" NODE_ADDON_API_DIR "${NODE_ADDON_API_DIR}")

# 2. 查找 Node.js 原生头文件路径
execute_process(
  COMMAND node -p "process.execPath.replace(/bin.*$/, 'include/node')"
  OUTPUT_VARIABLE NODE_INCLUDE_DIR
  OUTPUT_STRIP_TRAILING_WHITESPACE
)

# 3. 包含头文件路径
include_directories(
  ${NODE_INCLUDE_DIR} # node.h 所在目录
  ${NODE_ADDON_API_DIR} # napi.h 所在目录
  ${CMAKE_JS_INC}
)

# 添加子目录
add_subdirectory(native)
add_subdirectory(external/sqlite3)



# #############|test|################
if(WITH_TESTS)
  include(CTest)
  include(catch2)
  add_subdirectory(tests)
endif()
