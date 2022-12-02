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
		esp-tls
		bt
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
	${CMAKE_CURRENT_LIST_DIR}/tls.c
	${CMAKE_CURRENT_LIST_DIR}/i2c0.c
	${LIBMCU_ROOT}/ports/freertos/semaphore.c
	${CMAKE_SOURCE_DIR}/drivers/wifi/esp32.c
	${CMAKE_SOURCE_DIR}/drivers/ble/esp32.c
	${CMAKE_SOURCE_DIR}/ports/coreMQTT/mqtt.c
)

set(mapfile "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map")
target_compile_options(${PROJECT_EXECUTABLE} PRIVATE ${compile_options} -finstrument-functions)
target_include_directories(${PROJECT_EXECUTABLE}
	PRIVATE
		$ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos
		$ENV{IDF_PATH}/components/freertos/include/freertos
)
target_compile_definitions(${PROJECT_EXECUTABLE}
	PRIVATE
		WIFI_DEFAULT_INTERFACE=esp
		BLE_DEFAULT_INTERFACE=esp
)
target_compile_definitions(fpl_app
	PRIVATE
		WIFI_DEFAULT_INTERFACE=esp
		BLE_DEFAULT_INTERFACE=esp
)

include(FetchContent)
FetchContent_Declare(core_mqtt_src URL https://github.com/FreeRTOS/coreMQTT/archive/refs/tags/v2.1.1.zip)
FetchContent_Populate(core_mqtt_src)
include(${core_mqtt_src_SOURCE_DIR}/mqttFilePaths.cmake)
add_library(core_mqtt ${MQTT_SOURCES} ${MQTT_SERIALIZER_SOURCES})
target_compile_definitions(core_mqtt PUBLIC MQTT_DO_NOT_USE_CUSTOM_CONFIG)
target_include_directories(core_mqtt PUBLIC ${MQTT_INCLUDE_PUBLIC_DIRS} ${CMAKE_SOURCE_DIR}/include)

# Link the static libraries to the executable
target_link_libraries(${PROJECT_EXECUTABLE}
	idf::${BOARD}
	idf::freertos
	idf::spi_flash
	idf::nvs_flash
	idf::esp-tls
	idf::bt
	core_mqtt
	fpl_app
	-Wl,--cref
	-Wl,--Map=\"${mapfile}\"
)

set(idf_size ${python} $ENV{IDF_PATH}/tools/idf_size.py)
add_custom_target(size DEPENDS ${mapfile} COMMAND ${idf_size} ${mapfile})
add_custom_target(size-files DEPENDS ${mapfile} COMMAND ${idf_size} --files ${mapfile})
add_custom_target(size-components DEPENDS ${mapfile} COMMAND ${idf_size} --archives ${mapfile})

# Attach additional targets to the executable file for flashing,
# linker script generation, partition_table generation, etc.
idf_build_executable(${PROJECT_EXECUTABLE})
