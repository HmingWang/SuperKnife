
aux_source_directory(./src utils_src)
add_library(utils ${utils_src})
target_include_directories(utils PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../common
  ${CMAKE_CURRENT_SOURCE_DIR}/inc
)

target_link_libraries(utils PUBLIC compiler_inf)
