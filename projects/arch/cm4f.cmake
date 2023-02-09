# SPDX-License-Identifier: Apache-2.0

include(${CMAKE_SOURCE_DIR}/projects/arch/common.cmake)

add_compile_options(
	-mcpu=cortex-m4
	-mfloat-abi=hard
	-mfpu=fpv4-sp-d16
)
add_link_options(
	-mcpu=cortex-m4
	-mfloat-abi=hard
	-mfpu=fpv4-sp-d16
)
