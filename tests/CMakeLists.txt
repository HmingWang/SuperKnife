message(STATUS "单元测试组件")


include(Catch)
aux_source_directory(. test_src)
add_executable(tests ${test_src})
target_link_libraries(tests PRIVATE Catch2::Catch2WithMain xcore compiler_inf)
# target_link_libraries(tests PRIVATE Catch2::Catch2 xcore compiler_inf)


catch_discover_tests(tests)

# if(WIN32)
#     message(STATUS "当前平台：Windows")
#     add_custom_command(TARGET tests POST_BUILD
#         COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/libs/libcrypto-3-x64.dll ${CMAKE_BINARY_DIR}/tests/Debug
#         COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/libs/libssl-3-x64.dll ${CMAKE_BINARY_DIR}/tests/Debug
#         COMMAND_EXPAND_LISTS
#     )
# endif()