# SPDX-License-Identifier: Apache-2.0

cmake_minimum_required(VERSION 3.20.0)

set(BASEDIR ${CMAKE_CURRENT_LIST_DIR})
set(OUTDIR ${BASEDIR}/build)

if (DEFINED ESP_PLATFORM)
	if (NOT DEFINED BOARD)
		set(BOARD esp32)
	endif()
	set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-${BOARD}.cmake)
elseif (DEFINED WEST_PYTHON)
	set(OVERLAY_CONFIG ports/zephyr/fpl.conf)
	find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
endif()

project(${BOARD})

include(${BASEDIR}/projects/version.cmake)
include(${BASEDIR}/projects/app.cmake)

if (DEFINED ESP_PLATFORM)
	include(ports/esp-idf/${BOARD}.cmake)
elseif (DEFINED ZEPHYR_BASE)
	include(ports/zephyr/zephyr.cmake)
elseif (DEFINED BOARD)
	enable_language(C CXX ASM)
	include(projects/boards/${BOARD}.cmake)
else()
	message(FATAL_ERROR "Either of unknown platform or undefined board")
endif()
