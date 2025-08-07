
aux_source_directory(. SQLITE3_SRC)
add_library(sqlite3 ${SQLITE3_SRC})
target_include_directories(sqlite3 PUBLIC .)
target_link_libraries(sqlite3 PUBLIC msvc_inf)