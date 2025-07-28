message(STATUS "Tests include dir: ${CMAKE_CURRENT_SOURCE_DIR}/../addon")



include(Catch)
aux_source_directory(. test_src)
add_executable(tests ${test_src})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../addon)
catch_discover_tests(tests)


