# SPDX-License-Identifier: Apache-2.0

# Include for ESP-IDF build system functions
include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

idf_build_process(${BOARD}
	# try and trim the build; additional components
	# will be included as needed based on dependency tree
	#
	# although esptool_py does not generate static library,
	# processing the component is needed for flashing related
	# targets and file generation
	COMPONENTS
		${BOARD}
		freertos
		esptool_py
	SDKCONFIG_DEFAULTS
		"${CMAKE_CURRENT_LIST_DIR}/sdkconfig.defaults"
	BUILD_DIR
		${CMAKE_CURRENT_BINARY_DIR}
)

# project_description.json metadata file used for the flash and the monitor of
# idf.py to get the project information.
set(PROJECT_EXECUTABLE ${CMAKE_PROJECT_NAME}.elf)
set(PROJECT_BIN ${CMAKE_PROJECT_NAME}.bin)
set(build_components_json "[]")
set(build_component_paths_json "[]")
configure_file("${IDF_PATH}/tools/cmake/project_description.json.in"
	"${CMAKE_CURRENT_BINARY_DIR}/project_description.json")

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_executable(${PROJECT_EXECUTABLE}
	${CMAKE_CURRENT_LIST_DIR}/start.c
	${CMAKE_CURRENT_LIST_DIR}/board.c
	${CMAKE_CURRENT_LIST_DIR}/uart0.c
	${CMAKE_CURRENT_LIST_DIR}/cli.c
	${APP_SRCS}
)

set(mapfile "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map")
target_link_options(${PROJECT_EXECUTABLE} PRIVATE -Wl,--print-memory-usage)
target_compile_options(${PROJECT_EXECUTABLE} PRIVATE ${compile_options})
target_compile_definitions(${PROJECT_EXECUTABLE} PRIVATE ${APP_DEFS})
target_include_directories(${PROJECT_EXECUTABLE}
	PRIVATE
		${APP_INCS}
		$ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos
		$ENV{IDF_PATH}/components/freertos/include/freertos
)

# Link the static libraries to the executable
target_link_libraries(${PROJECT_EXECUTABLE}
	idf::${BOARD}
	idf::freertos
	idf::spi_flash
	idf::nvs_flash
	-Wl,--cref
	-Wl,--Map=\"${mapfile}\"
)

# Attach additional targets to the executable file for flashing,
# linker script generation, partition_table generation, etc.
idf_build_executable(${PROJECT_EXECUTABLE})
