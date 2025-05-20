cmake_minimum_required(VERSION 3.10)

set(name backend)
project(${name})

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_DEBUG_POSTFIX "_d")
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
set(WITH_TESTS true)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

if(MSVC)
    include(msvc)
endif()
include(info)
info()

aux_source_directory(./src src)
add_library(${name} SHARED ${src})
target_compile_definitions(${name} PRIVATE XCORE_API_EXPORT)
target_include_directories(${name} PRIVATE ./src)

# #############|test|################
if(WITH_TESTS)
    include(CTest)
    include(catch2)
    add_subdirectory(tests)
endif()