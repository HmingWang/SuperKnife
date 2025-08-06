

aux_source_directory(. SRC_LIST)

# 定义 Node.js 原生模块
add_library(addon SHARED ${SRC_LIST} ${CMAKE_JS_SRC})
target_include_directories(addon PRIVATE
  ${CMAKE_JS_INC}
  ${NODE_INCLUDE_DIR}
  ${NODE_ADDON_API_DIR}
  ${OPENSSL_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/crypto
  ${CMAKE_SOURCE_DIR}/external/sqlite3
)

# 链接库
target_link_libraries(addon PRIVATE
  ${CMAKE_JS_LIB}
  sqlite3
  crypto
)

message(STATUS "Node.js include dir: ${NODE_INCLUDE_DIR}")
message(STATUS "Node-addon-api include dir: ${NODE_ADDON_API_DIR}")

# 设置输出名称和位置
set_target_properties(addon PROPERTIES PREFIX "" SUFFIX ".node")

if(MSVC)
  target_compile_options(addon PRIVATE "/utf-8")
endif()
