# SPDX-License-Identifier: MIT

set(MBEDTLS_ROOT ${CMAKE_SOURCE_DIR}/external/mbedtls)

option(ENABLE_TESTING OFF)
option(ENABLE_PROGRAMS OFF)
add_subdirectory(${MBEDTLS_ROOT} mbedtls)

target_compile_definitions(mbedtls PUBLIC MBEDTLS_USER_CONFIG_FILE=\"${CMAKE_CURRENT_LIST_DIR}/config.h\")
target_compile_definitions(mbedx509 PUBLIC MBEDTLS_USER_CONFIG_FILE=\"${CMAKE_CURRENT_LIST_DIR}/config.h\")
target_compile_definitions(mbedcrypto PUBLIC MBEDTLS_USER_CONFIG_FILE=\"${CMAKE_CURRENT_LIST_DIR}/config.h\")
target_compile_options(mbedcrypto PUBLIC "-Wno-error=return-type" -Wno-error)
