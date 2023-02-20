# SPDX-License-Identifier: Apache-2.0

include(${CMAKE_SOURCE_DIR}/projects/arch/common.cmake)

add_compile_options(-mcpu=cortex-m3)
add_link_options(-mcpu=cortex-m3)
