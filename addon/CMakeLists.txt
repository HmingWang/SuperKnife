
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
  # ${CMAKE_SOURCE_DIR}/external/xcore/crypto/inc # base64.h 所在目录
  # ${CMAKE_SOURCE_DIR}/external/xcore/dbtools # sqlitedb.h 所在目录
  # ${CMAKE_SOURCE_DIR}/external/xcore/dbtools/inc # dbtools 头文件目录
  # ${CMAKE_SOURCE_DIR}/external/xcore/common # 通用头文件目录
  # ${CMAKE_SOURCE_DIR}/external/xcore/utils/inc # exceptions.h 所在目录
)

aux_source_directory(. API_SRC)
add_library(addon SHARED ${API_SRC} ${CMAKE_JS_SRC})
target_link_libraries(addon 
    PRIVATE 
    ${CMAKE_JS_LIB}
    xCore::crypto
    xCore::log
    xCore::utils
    xCore::dbtools
    xCore::sqlite3)
target_include_directories(addon PUBLIC 
    ${CMAKE_JS_INC}   
    ${NODE_INCLUDE_DIR}
    ${NODE_ADDON_API_DIR})

# 设置输出名称和位置
set_target_properties(addon PROPERTIES PREFIX "" SUFFIX ".node")