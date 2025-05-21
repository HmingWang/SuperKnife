find_package(OpenSSL REQUIRED)


# 定义 Node.js 原生模块
add_library(auth SHARED auth.cpp)

# 链接库
target_link_libraries(auth PRIVATE
  OpenSSL::SSL
  OpenSSL::Crypto
)

target_include_directories(auth PRIVATE
  ${CMAKE_SOURCE_DIR}/external/sqlite3
  ${NODE_ADDON_API_DIR}
)
