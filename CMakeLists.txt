# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.24.0")
	cmake_policy(SET CMP0135 NEW)
endif()

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(TARGET_PLATFORM "" CACHE STRING "Supported Target Platforms: madi_nrf52840, host")
if (NOT TARGET_PLATFORM)
	if (DEFINED ENV{TARGET_PLATFORM})
		set(TARGET_PLATFORM $ENV{TARGET_PLATFORM})
	elseif (DEFINED ENV{ZEPHYR_TOOLCHAIN_VARIANT})
		set(TARGET_PLATFORM ${BOARD})
	else()
		get_property(TARGET_PLATFORM_HELP CACHE TARGET_PLATFORM PROPERTY HELPSTRING)
		message(FATAL_ERROR "TARGET_PLATFORM is not set. Use -DTARGET_PLATFORM=<platform> or set the TARGET_PLATFORM environment variable. ${TARGET_PLATFORM_HELP}")
	endif()
endif()

# Strip board qualifiers (e.g. "madi_nrf52840/nrf52840" -> "madi_nrf52840").
# BOARD from CMakeCache on re-runs or from sysbuild may include "/qualifier" or "@revision".
# Must run unconditionally so both first-run and re-run produce a clean platform name.
if (TARGET_PLATFORM MATCHES "[/@]")
	string(REGEX REPLACE "[/@].*" "" TARGET_PLATFORM "${TARGET_PLATFORM}")
endif()
if (DEFINED ENV{ZEPHYR_TOOLCHAIN_VARIANT})
	include(${CMAKE_SOURCE_DIR}/projects/platforms/zephyr.cmake)
	# In NCS sysbuild first run, BOARD is empty before find_package(Zephyr).
	# After include(zephyr.cmake), BOARD is set from sysbuild cache.
	if (NOT TARGET_PLATFORM AND BOARD)
		string(REGEX REPLACE "[/@].*" "" TARGET_PLATFORM "${BOARD}")
	endif()
elseif (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
	set(CMAKE_TOOLCHAIN_FILE ${CMAKE_SOURCE_DIR}/projects/arch/arm/arm-none-eabi-gcc.cmake)
endif()

set(PLATFORM_PRE_CMAKE_FILE "${CMAKE_SOURCE_DIR}/projects/platforms/${TARGET_PLATFORM}-pre.cmake")
if (EXISTS ${PLATFORM_PRE_CMAKE_FILE})
	include(${PLATFORM_PRE_CMAKE_FILE})
endif()

project(madi)
enable_language(C CXX ASM)

include(${CMAKE_SOURCE_DIR}/projects/version.cmake)
include(${CMAKE_SOURCE_DIR}/projects/warnings.cmake)
include(${CMAKE_SOURCE_DIR}/projects/external.cmake)
include(${CMAKE_SOURCE_DIR}/projects/sources.cmake)

set(PLATFORM_CMAKE_FILE "${CMAKE_SOURCE_DIR}/projects/platforms/${TARGET_PLATFORM}.cmake")
if (NOT EXISTS ${PLATFORM_CMAKE_FILE})
	message(FATAL_ERROR "Platform CMake file not found: ${PLATFORM_CMAKE_FILE}")
endif()
include(${PLATFORM_CMAKE_FILE})
