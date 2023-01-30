# SPDX-License-Identifier: Apache-2.0

# Include for ESP-IDF build system functions
include($ENV{IDF_PATH}/tools/cmake/idf.cmake)

AUX_SOURCE_DIRECTORY(${CMAKE_CURRENT_LIST_DIR} PORT_SRCS)
set(ESP_COMPONENTS freertos esptool_py esp-tls bt)

if ($ENV{IDF_VERSION} VERSION_GREATER_EQUAL "5.0.0")
	list(APPEND ESP_COMPONENTS esp_adc)
else()
	list(APPEND ESP_COMPONENTS esp_adc_cal)
endif()

idf_build_process(${BOARD}
	COMPONENTS
		${ESP_COMPONENTS}
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

include(FetchContent)
FetchContent_Declare(core_mqtt_src URL https://github.com/FreeRTOS/coreMQTT/archive/refs/tags/v2.1.1.zip)
FetchContent_Populate(core_mqtt_src)
include(${core_mqtt_src_SOURCE_DIR}/mqttFilePaths.cmake)
add_library(core_mqtt ${MQTT_SOURCES} ${MQTT_SERIALIZER_SOURCES})
target_compile_definitions(core_mqtt PUBLIC MQTT_DO_NOT_USE_CUSTOM_CONFIG)
target_include_directories(core_mqtt PUBLIC ${MQTT_INCLUDE_PUBLIC_DIRS} ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(pmqtt core_mqtt)
target_link_libraries(pble idf::bt)
target_link_libraries(pwifi idf::esp_wifi)
target_link_libraries(pl4 idf::esp-tls)

target_include_directories(libmcu PUBLIC
	${CMAKE_SOURCE_DIR}/external/libmcu/modules/common/include/libmcu/posix)

set(LIBMCU_ROOT ${PROJECT_SOURCE_DIR}/external/libmcu)

add_executable(${PROJECT_EXECUTABLE}
	${APP_SRCS}
	${PORT_SRCS}
	${LIBMCU_ROOT}/ports/freertos/semaphore.c
	${LIBMCU_ROOT}/ports/esp-idf/board.c
)

set(mapfile "${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.map")
target_compile_options(${PROJECT_EXECUTABLE} PRIVATE ${compile_options})
target_compile_definitions(${PROJECT_EXECUTABLE}
	PRIVATE
		ESP_PLATFORM=1
		xPortIsInsideInterrupt=xPortInIsrContext
		${APP_DEFS}
)
target_include_directories(${PROJECT_EXECUTABLE}
	PRIVATE
		$ENV{IDF_PATH}/components/freertos/FreeRTOS-Kernel/include/freertos
		$ENV{IDF_PATH}/components/freertos/include/freertos
		${CMAKE_CURRENT_LIST_DIR}
		${APP_INCS}
)

# Link the static libraries to the executable
target_link_libraries(${PROJECT_EXECUTABLE}
	idf::freertos
	idf::spi_flash
	idf::nvs_flash
	idf::driver

	libmcu
	pble
	pwifi
	pl4
	pmqtt

	-Wl,--cref
	-Wl,--Map=\"${mapfile}\"
)
if ($ENV{IDF_VERSION} VERSION_GREATER_EQUAL "5.0.0")
target_link_libraries(${PROJECT_EXECUTABLE} idf::esp_adc)
else()
target_link_libraries(${PROJECT_EXECUTABLE} idf::esp_adc_cal)
endif()

set(idf_size ${python} $ENV{IDF_PATH}/tools/idf_size.py)
add_custom_target(size DEPENDS ${mapfile} COMMAND ${idf_size} ${mapfile})
add_custom_target(size-files DEPENDS ${mapfile} COMMAND ${idf_size} --files ${mapfile})
add_custom_target(size-components DEPENDS ${mapfile} COMMAND ${idf_size} --archives ${mapfile})

# Attach additional targets to the executable file for flashing,
# linker script generation, partition_table generation, etc.
idf_build_executable(${PROJECT_EXECUTABLE})
