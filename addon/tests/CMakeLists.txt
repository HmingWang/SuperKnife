include(Catch)
aux_source_directory(. test_src)
add_executable(tests ${test_src})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain crypto)
target_include_directories(tests PRIVATE
${CMAKE_CURRENT_SOURCE_DIR}/../crypto
${OPENSSL_INCLUDE_DIR})
catch_discover_tests(tests)
