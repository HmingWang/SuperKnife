message(STATUS "编译器相关配置")

add_library(compiler_inf INTERFACE)

if(MSVC)
    message(STATUS "MSVC编译器增加utf8选项")
    target_compile_options(compiler_inf INTERFACE "/utf-8")
endif()