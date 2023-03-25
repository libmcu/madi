# SPDX-License-Identifier: Apache-2.0

include(${CMAKE_SOURCE_DIR}/projects/arch/cm4f.cmake)

set(TARGET_PLATFORM nrf52)
set(PLATFORM_SPECIFIC_DIR ${CMAKE_SOURCE_DIR}/ports/nrf5-sdk)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME})
add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${PROJECT_NAME})
add_custom_target(flash DEPENDS ${PROJECT_NAME}.bin)
add_custom_target(flash_jlink DEPENDS ${PROJECT_NAME}.hex)
add_custom_target(flash_pyocd DEPENDS ${PROJECT_NAME}.hex)
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
		# specify path with --path or serial with --serial when more
		# than one device connected
		dfu-util --device 1209:e001 --alt 0 --download ${PROJECT_NAME}.bin
)

add_custom_command(TARGET flash_jlink
	USES_TERMINAL
	COMMAND
		nrfjprog -f nrf52 --program ${PROJECT_NAME}.hex --sectorerase --verify
	COMMAND
		nrfjprog -f nrf52 --reset
)

add_custom_command(TARGET flash_pyocd
	USES_TERMINAL COMMAND
		pyocd flash -t nrf52840 ${PROJECT_NAME}.hex
)

add_custom_command(TARGET gdb
	USES_TERMINAL COMMAND
		pyocd gdbserver -t nrf52840
)
