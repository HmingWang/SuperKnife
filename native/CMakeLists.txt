find_package(OpenSSL REQUIRED)
find_package(SQLite3 REQUIRED)

include(FetchContent)

if(not Sqlite3_FOUND)
  message(FATAL_ERROR "SQLite3 not found")
  FetchContent_Declare(SQLite
    URL https://sqlite.org/2021/sqlite-amalgamation-3350300.zip
  )
  FetchContent_Populate(SQLite)
endif()

if(sqlite3_FOUND)
  message(STATUS "SQLite3 found")
  include_directories(${sqlite3_SOURCE_DIR})
endif()

# 定义 Node.js 原生模块
add_library(auth SHARED auth.cpp)

# 链接库
target_link_libraries(auth PRIVATE
  OpenSSL::SSL
  OpenSSL::Crypto
  SQLite::SQLite3
)

# Node.js 插件特定设置
set_target_properties(auth PROPERTIES
  PREFIX ""
  SUFFIX ".node"
  OUTPUT_NAME "auth"
)
