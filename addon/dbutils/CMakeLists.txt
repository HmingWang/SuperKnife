message(STATUS "数据库工具组件")

aux_source_directory(. db_src)
add_library(dbutils sqlite3.hpp)
target_link_libraries(dbutils PUBLIC sqlite3)
set_target_properties(dbutils PROPERTIES LINKER_LANGUAGE CXX)