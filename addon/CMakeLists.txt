find_package(OpenSSL REQUIRED)


set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")




aux_source_directory(. SRC_LIST)

# 定义 Node.js 原生模块
add_library(addon SHARED ${SRC_LIST} ${CMAKE_JS_SRC})


# 链接库
target_link_libraries(addon PRIVATE
  OpenSSL::SSL
  OpenSSL::Crypto
  ${CMAKE_JS_LIB}
  sqlite3
)

message(STATUS "Node.js include dir: ${NODE_INCLUDE_DIR}")
message(STATUS "Node-addon-api include dir: ${NODE_ADDON_API_DIR}")

target_include_directories(addon
  PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
  PRIVATE ${CMAKE_SOURCE_DIR}/external/sqlite3
  ${CMAKE_JS_INC}
)

# 设置输出名称和位置
set_target_properties(addon PROPERTIES PREFIX "" SUFFIX ".node")

if(MSVC)
  target_compile_options(addon PRIVATE "/utf-8")
endif()
