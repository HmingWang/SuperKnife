find_package(OpenSSL REQUIRED)

if(NOT OpenSSL_FOUND)
  message(FATAL_ERROR "OpenSSL not found")
else()
  # set(OPENSSL_LIB_DIR /opt/homebrew/Cellar/openssl@3/3.5.1/lib)
  message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
  message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
  message(STATUS "OpenSSL library dir: ${OPENSSL_LIB_DIR}")
endif()

set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

message(STATUS "cmake-js include : ${CMAKE_JS_INC}")
message(STATUS "cmake-js library : ${CMAKE_JS_LIB}")
message(STATUS "cmake-js source  : ${CMAKE_JS_SRC}")

aux_source_directory(. SRC_LIST)

# 定义 Node.js 原生模块
add_library(addon SHARED ${SRC_LIST} ${CMAKE_JS_SRC})
target_include_directories(addon
  PRIVATE ${CMAKE_JS_INC}
  PRIVATE ${NODE_INCLUDE_DIR}
  PRIVATE ${NODE_ADDON_API_DIR}
  PRIVATE ${OPENSSL_INCLUDE_DIR}
)

target_link_directories(addon PRIVATE ${OPENSSL_LIB_DIR})

# 链接库
target_link_libraries(addon PRIVATE
  OpenSSL::SSL OpenSSL::Crypto
  ${CMAKE_JS_LIB}
  sqlite3
)

message(STATUS "Node.js include dir: ${NODE_INCLUDE_DIR}")
message(STATUS "Node-addon-api include dir: ${NODE_ADDON_API_DIR}")

target_include_directories(addon
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/external/sqlite3
  PRIVATE ${CMAKE_JS_INC}
)

# 设置输出名称和位置
set_target_properties(addon PROPERTIES PREFIX "" SUFFIX ".node")

if(MSVC)
  target_compile_options(addon PRIVATE "/utf-8")
endif()
