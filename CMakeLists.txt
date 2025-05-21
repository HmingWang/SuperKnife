cmake_minimum_required(VERSION 3.22)
set(name native)
project(${name})

set(CMAKE_CXX_STANDARD 17)

# 设置输出目录到 Node.js 能访问的位置
set(OUTPUT_DIR ${CMAKE_BINARY_DIR}/../build/Release)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# 查找 Node.js 和 Electron 头文件
execute_process(COMMAND node -p "require('node-addon-api').include"
                OUTPUT_VARIABLE NODE_ADDON_API_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)

execute_process(COMMAND node -p "process.execPath.replace(/bin.*$/, 'include/node')"
                OUTPUT_VARIABLE NODE_INCLUDE_DIR
                OUTPUT_STRIP_TRAILING_WHITESPACE)

include_directories(
    ${NODE_INCLUDE_DIR}
    ${NODE_ADDON_API_DIR}
)

# 添加子目录
add_subdirectory(native)
