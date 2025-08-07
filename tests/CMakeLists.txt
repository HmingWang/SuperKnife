message(STATUS "单元测试组件")


include(Catch)
aux_source_directory(. test_src)
add_executable(tests ${test_src})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain crypto msvc_inf)

catch_discover_tests(tests)