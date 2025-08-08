message(STATUS "加解密组件")

include(openssl)

aux_source_directory(./src crypto_src)
add_library(crypto ${crypto_src})
target_include_directories(crypto PUBLIC ${CMAKE_CURRENT_SOURCE_DIR} ${OPENSSL_INCLUDE_DIR} ./inc)
target_link_directories(crypto PUBLIC ${OPENSSL_LIB_DIR})
target_link_libraries(crypto PUBLIC OpenSSL::SSL OpenSSL::Crypto)
