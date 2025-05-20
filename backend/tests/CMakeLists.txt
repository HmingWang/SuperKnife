# catch2
include(Catch)


add_library(dll SHARED IMPORTED)
set_target_properties(dll PROPERTIES
	IMPORTED_LOCATION ${CMAKE_BINARY_DIR}/${name}.dll
	IMPORTED_IMPLIB ${CMAKE_BINARY_DIR}/${name}.lib
)



aux_source_directory(. test_src)
add_executable(tests ${test_src})

target_link_libraries(tests PRIVATE dll Catch2::Catch2WithMain)

catch_discover_tests(tests)