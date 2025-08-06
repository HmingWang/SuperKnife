message(STATUS "finding OpenSSL... ${OpenSSL_ROOT_DIR}")

find_package(OpenSSL REQUIRED)

if(NOT OpenSSL_FOUND)
  message(FATAL_ERROR "OpenSSL not found")
else()
  message(STATUS "OpenSSL found: ${OPENSSL_VERSION}")
  message(STATUS "OpenSSL include dir: ${OPENSSL_INCLUDE_DIR}")
  message(STATUS "OpenSSL libraries: ${OPENSSL_LIBRARIES}")
  message(STATUS "OpenSSL library dir: ${OPENSSL_LIB_DIR}")
endif()
