# SPDX-License-Identifier: Apache-2.0

include(${CMAKE_SOURCE_DIR}/projects/arch/cm4f.cmake)

set(TARGET_PLATFORM stm32)
set(PLATFORM_SPECIFIC_DIR ${CMAKE_SOURCE_DIR}/ports/stm32/g4)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME})
add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${PROJECT_NAME})
add_custom_target(flash DEPENDS ${PROJECT_NAME}.bin)
add_custom_target(flash_jlink DEPENDS ${PROJECT_NAME}.bin)
add_custom_target(gdb DEPENDS ${PROJECT_NAME})

add_custom_command(TARGET ${PROJECT_NAME}.hex
	COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${PROJECT_NAME}>
			${PROJECT_NAME}.hex
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET ${PROJECT_NAME}.bin
	COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${PROJECT_NAME}>
			${PROJECT_NAME}.bin
	WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
)

add_custom_command(TARGET flash
	USES_TERMINAL COMMAND
		dfu-util --alt 0 --dfuse-address 0x08000000 --download ${PROJECT_NAME}.bin
)

add_custom_command(TARGET flash_jlink
	USES_TERMINAL
	COMMAND
		echo \"r\\nloadbin ${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}.bin, 0x08000000\\nr\\nq\" > ${PROJECT_NAME}.jlink
	COMMAND
		JLinkExe -if swd -device stm32g473ce -speed 4000 -CommanderScript ${PROJECT_NAME}.jlink
)

add_custom_command(TARGET gdb
	USES_TERMINAL COMMAND
		pyocd gdbserver -t stm32g473ce
)
