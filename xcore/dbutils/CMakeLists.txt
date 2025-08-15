info(STATUS "数据库工具组件")

aux_source_directory(. DB_SRC)

add_library(dbutils ${DB_SRC})
target_link_libraries(dbutils PUBLIC sqlite3)
