

aux_source_directory(. api_src)
add_library(api ${api_src} ${CMAKE_JS_SRC})
target_link_libraries(api PRIVATE ${CMAKE_JS_LIB} db crypto)
target_include_directories(api PRIVATE 
    ${CMAKE_JS_INC}   
    ${NODE_INCLUDE_DIR}
    ${NODE_ADDON_API_DIR}
)

# 设置输出名称和位置
set_target_properties(api PROPERTIES PREFIX "" SUFFIX ".node")