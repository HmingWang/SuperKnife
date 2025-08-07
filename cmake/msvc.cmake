message(STATUS "MSVC相关配置")

add_library(msvc_inf INTERFACE)

if(MSVC)
    target_compile_options(msvc_inf INTERFACE "/utf-8")
endif()