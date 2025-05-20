# catch2
include(Catch)


if(WIN32)
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../backend_d.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
elseif(APPLE)
    file(COPY ${CMAKE_CURRENT_BINARY_DIR}/../libbackend_d.dylib DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
else()
endif()

aux_source_directory(. test_src)
add_executable(tests ${test_src})

target_link_libraries(tests PRIVATE Catch2::Catch2WithMain)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../src)

catch_discover_tests(tests)