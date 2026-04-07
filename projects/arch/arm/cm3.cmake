# SPDX-License-Identifier: MIT

include(${CMAKE_SOURCE_DIR}/projects/arch/arm/common.cmake)

add_library(arch_options INTERFACE)
target_compile_options(arch_options INTERFACE -mcpu=cortex-m3)
target_link_options(arch_options INTERFACE -mcpu=cortex-m3)
target_link_libraries(arch_options INTERFACE arch_common_options)
