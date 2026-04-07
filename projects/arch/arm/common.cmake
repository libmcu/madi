# SPDX-License-Identifier: MIT

add_library(arch_common_options INTERFACE)
target_compile_options(arch_common_options INTERFACE
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

target_link_options(arch_common_options INTERFACE
	-mthumb
	-mabi=aapcs
	--specs=nano.specs
	--specs=nosys.specs

	-flto
	-Wl,--gc-sections
	-Wl,--print-memory-usage
)

target_link_options(arch_common_options INTERFACE
	-Wl,--cref
	-Wl,--Map=\"${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map\"
)
