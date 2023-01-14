# SPDX-License-Identifier: Apache-2.0

set(PLATFORM_SPECIFIC_DIR ${CMAKE_SOURCE_DIR}/ports/stm32/f1)

include(${BASEDIR}/projects/arch/cm3.cmake)

add_executable(${PROJECT_NAME})
target_include_directories(${PROJECT_NAME} PRIVATE ${APP_INCS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${APP_DEFS})

target_link_libraries(${PROJECT_NAME} PRIVATE c nosys m fpl_app)

add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${PROJECT_NAME})
add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${PROJECT_NAME})
add_custom_target(flash DEPENDS ${PROJECT_NAME}.hex)
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
		pyocd flash -t stm32f103c8 ${PROJECT_NAME}.hex
)

add_custom_command(TARGET gdb
	USES_TERMINAL COMMAND
		pyocd gdbserver -t stm32f103c8
)
