# SPDX-License-Identifier: Apache-2.0

add_subdirectory(external/libmcu)
add_subdirectory(external/pble)
add_subdirectory(external/pwifi)
add_subdirectory(external/pl4)
add_subdirectory(external/pmqtt)
add_subdirectory(external/bq25180)
add_subdirectory(ports/tinyusb)
add_subdirectory(ports/lwip)

target_compile_definitions(libmcu PUBLIC
	METRICS_USER_DEFINES=\"${PROJECT_SOURCE_DIR}/include/metrics.def\"
	_POSIX_THREADS
	_POSIX_C_SOURCE=200809L
)
