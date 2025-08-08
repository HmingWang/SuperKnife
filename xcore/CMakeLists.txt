add_subdirectory(crypto)
add_subdirectory(dbutils)

add_library(xcore INTERFACE)
target_link_libraries(xcore INTERFACE crypto dbutils compiler_inf)