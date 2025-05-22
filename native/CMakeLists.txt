find_package(OpenSSL REQUIRED)


# 定义 Node.js 原生模块
add_library(auth SHARED auth.cpp)


# 链接库
target_link_libraries(auth PRIVATE
  OpenSSL::SSL
  OpenSSL::Crypto
  ${CMAKE_JS_LIB}
  sqlite3
)

message(STATUS "Node.js include dir: ${NODE_INCLUDE_DIR}")
message(STATUS "Node-addon-api include dir: ${NODE_ADDON_API_DIR}")

target_include_directories(auth
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/external/sqlite3
)

# 设置输出名称和位置
set_target_properties(auth PROPERTIES PREFIX "" SUFFIX ".node")
