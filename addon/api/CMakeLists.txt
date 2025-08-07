
include(msvc)

aux_source_directory(. API_SRC)
add_library(api SHARED ${API_SRC} ${CMAKE_JS_SRC})
target_link_libraries(api PUBLIC ${CMAKE_JS_LIB} dbutils crypto msvc_inf)
target_include_directories(api PUBLIC 
    ${CMAKE_JS_INC}   
    ${NODE_INCLUDE_DIR}
    ${NODE_ADDON_API_DIR}
)

# 设置输出名称和位置
set_target_properties(api PROPERTIES PREFIX "" SUFFIX ".node")