cmake_minimum_required(VERSION 3.10)

message(STATUS "配置信息： ${CMAKE_SOURCE_DIR}")

set(name backend)
project(${name})


set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(WITH_TESTS true)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

aux_source_directory(. src)
add_library(${name} SHARED ${src})

##############|test|################
if(WITH_TESTS)
    include(CTest)
    include(catch2)
    add_subdirectory(tests)
endif()