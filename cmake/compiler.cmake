message(STATUS "编译器相关配置")

if(MSVC)
    message(STATUS "Adding UTF-8 support for MSVC")
    # target_compile_options(compiler_inf INTERFACE
    #     "/utf-8" # 使用 UTF-8 编码
    #     "/wd4819" # 禁用 4819 警告
    #     "/w14640" # 启用 Unicode 相关警告
    # )

    # 设置源文件编码
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

    # 设置运行时库
    if(BUILD_SHARED_LIBS)
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    else()
        set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    endif()
endif()