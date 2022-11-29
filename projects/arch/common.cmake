# SPDX-License-Identifier: Apache-2.0

add_compile_options(
	-mthumb
	-mabi=aapcs

	-std=c99
	-static
	-ffreestanding
	-fno-builtin
	-fno-common
	-ffunction-sections
	-fdata-sections
	-fstack-usage
	-Os
	-g3
)

add_link_options(
	-mthumb
	-mabi=aapcs
	--specs=nano.specs
	--specs=nosys.specs

	-flto
	-Wl,--gc-sections
	-Wl,--print-memory-usage
)

link_libraries(
	-Wl,--cref
	-Wl,--Map=\"${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map\"
)
