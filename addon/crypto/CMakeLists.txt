include(openssl)

aux_source_directory(. crypto_src)
add_library(crypto STATIC ${crypto_src})
target_include_directories(crypto PRIVATE ${CMAKE_CURRENT_SOURCE_DIR} ${OPENSSL_INCLUDE_DIR})
target_link_directories(crypto PRIVATE ${OPENSSL_LIB_DIR})
target_link_libraries(crypto PRIVATE OpenSSL::SSL OpenSSL::Crypto)
