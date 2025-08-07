cmake_minimum_required(VERSION 3.22)
set(name native)
project(${name})

set(CMAKE_CXX_STANDARD 23)
option(WITH_TESTS "编译单元测试案例" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(msvc)

# 添加子目录
add_subdirectory(addon)
add_subdirectory(external/sqlite3)



# #############|test|################
if(WITH_TESTS)
  include(CTest)
  include(catch2)
  add_subdirectory(tests)
endif()



