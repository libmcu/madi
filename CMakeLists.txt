cmake_minimum_required(VERSION 3.20.0)

set(BASEDIR ${CMAKE_CURRENT_LIST_DIR})
set(OUTDIR ${BASEDIR}/build)

if (DEFINED ESP_PLATFORM)
	if (NOT DEFINED DEVICE)
		set(DEVICE esp32)
	endif()
	set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-${DEVICE}.cmake)
elseif (DEFINED WEST_PYTHON)
	set(DEVICE ${BOARD})
	set(OVERLAY_CONFIG ports/zephyr/fpl.conf)
	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
endif()

project(${DEVICE})

include(${BASEDIR}/projects/app.cmake)

if (DEFINED ESP_PLATFORM)
	include(ports/esp-idf/${DEVICE}.cmake)
elseif (DEFINED ZEPHYR_BASE)
	include(ports/zephyr/zephyr.cmake)
endif()
