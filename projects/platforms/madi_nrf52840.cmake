# SPDX-License-Identifier: MIT

if (DEFINED ENV{ZEPHYR_TOOLCHAIN_VARIANT})
	list(REMOVE_ITEM APP_SRCS src/logging.c)

	set(ports ports/zephyr)
	foreach(dir ${ports})
		file(GLOB_RECURSE ${dir}_SRCS RELATIVE ${CMAKE_SOURCE_DIR} ${dir}/*.c)
		file(GLOB_RECURSE ${dir}_CPP_SRCS RELATIVE ${CMAKE_SOURCE_DIR} ${dir}/*.cpp)
		list(APPEND PORT_SRCS ${${dir}_SRCS} ${${dir}_CPP_SRCS})
	endforeach()

	set(LIBMCU_ROOT ${CMAKE_SOURCE_DIR}/external/libmcu)
	list(APPEND PORT_SRCS
		${LIBMCU_ROOT}/ports/zephyr/timext.c
	)

	target_sources(app PRIVATE
		${APP_SRCS}
		${PORT_SRCS}
	)

	target_compile_definitions(app
		PRIVATE
			${APP_DEFS}
	)

	target_include_directories(app
		PRIVATE
			${APP_INCS}
			${CMAKE_CURRENT_LIST_DIR}
	)

	target_link_libraries(app
		PRIVATE
			libmcu
	)

	target_link_libraries(libmcu PRIVATE zephyr_interface)

else()
	include(${CMAKE_SOURCE_DIR}/projects/arch/arm/cm4f.cmake)

	list(APPEND APP_SRCS
		${CMAKE_SOURCE_DIR}/external/libmcu/ports/armcm/fault.c
		${CMAKE_SOURCE_DIR}/external/libmcu/ports/armcm/assert.c
	)

	set(TARGET_PLATFORM nrf52)
	set(PLATFORM_SPECIFIC ${CMAKE_SOURCE_DIR}/ports/${TARGET_PLATFORM})
	set(LD_SCRIPT ${PLATFORM_SPECIFIC}/nrf52840.ld)

	add_library(platform_options INTERFACE)
	target_compile_definitions(platform_options INTERFACE ${APP_DEFS})
	target_include_directories(platform_options INTERFACE ${APP_INCS})
	target_compile_options(platform_options INTERFACE
		$<$<COMPILE_LANGUAGE:C>:-include libmcu/assert.h>
		$<$<COMPILE_LANGUAGE:CXX>:-include libmcu/assert.h>
	)
	target_link_libraries(platform_options INTERFACE arch_options)

	set(elf_file ${PROJECT_NAME}.elf)
	add_executable(${elf_file} ${APP_SRCS})
	target_link_options(${elf_file} PRIVATE -T${LD_SCRIPT})
	target_link_libraries(${elf_file} PRIVATE
		platform_options
		warnings

		-Wl,--cref
		-Wl,--Map=\"${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map\"

		${TARGET_PLATFORM}
		rtt
	)

	target_include_directories(libmcu PUBLIC
		${CMAKE_SOURCE_DIR}/external/libmcu/modules/common/include/libmcu/posix)
	target_link_libraries(libmcu PUBLIC platform_options)

	add_subdirectory(ports/rtt)
	target_link_libraries(rtt PUBLIC libmcu)
	target_link_libraries(rtt PUBLIC platform_options)

	add_subdirectory(${PLATFORM_SPECIFIC})
	target_link_libraries(${TARGET_PLATFORM} PUBLIC libmcu)
	target_link_libraries(${TARGET_PLATFORM} PUBLIC platform_options)

	add_custom_target(${PROJECT_NAME}.bin ALL DEPENDS ${elf_file})
	add_custom_target(${PROJECT_NAME}.hex ALL DEPENDS ${elf_file})
	add_custom_target(flash DEPENDS ${PROJECT_NAME}.hex)
	add_custom_target(flash_softdevice)
	add_custom_target(flash_usb DEPENDS ${PROJECT_NAME}.bin)
	add_custom_target(gdb DEPENDS ${elf_file})

	add_custom_command(TARGET ${PROJECT_NAME}.hex POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} -O ihex $<TARGET_FILE:${elf_file}>
				${PROJECT_NAME}.hex
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

	add_custom_command(TARGET ${PROJECT_NAME}.bin POST_BUILD
		COMMAND ${CMAKE_OBJCOPY} -O binary $<TARGET_FILE:${elf_file}>
				${PROJECT_NAME}.bin
		WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
	)

	add_custom_command(TARGET flash_usb POST_BUILD
		USES_TERMINAL COMMAND
			dfu-util --device 1209:e001 --alt 0 --download ${PROJECT_NAME}.bin
	)

	add_custom_command(TARGET flash POST_BUILD
		USES_TERMINAL
		COMMAND
			nrfjprog -f nrf52 --program ${PROJECT_NAME}.hex --sectorerase --verify
		COMMAND
			nrfjprog -f nrf52 --reset
	)

	add_custom_command(TARGET flash_softdevice POST_BUILD
		USES_TERMINAL
		COMMAND
			nrfjprog -f nrf52 --program ${CMAKE_SOURCE_DIR}/external/nRF5_SDK_17.1.0_ddde560/components/softdevice/s140/hex/s140_nrf52_7.2.0_softdevice.hex --sectorerase --verify
	)

	add_custom_command(TARGET gdb POST_BUILD
		USES_TERMINAL COMMAND
			pyocd gdbserver -t nrf52840
	)
endif()
