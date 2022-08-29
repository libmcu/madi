cmake_minimum_required(VERSION 3.16)

if (DEFINED ESP_PLATFORM)
	if (NOT DEFINED DEVICE)
		set(DEVICE esp32)
	endif()
	set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-${DEVICE}.cmake)
endif()

project(${DEVICE})

set(BASEDIR ${CMAKE_CURRENT_LIST_DIR})
set(OUTDIR ${BASEDIR}/build)

if (DEFINED ESP_PLATFORM)
	include(ports/esp-idf/${DEVICE}.cmake)
endif()
