# catch2
include(Catch)



file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../backend_d.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})


aux_source_directory(. test_src)
add_executable(tests ${test_src})

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)

catch_discover_tests(tests)