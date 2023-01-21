# SPDX-License-Identifier: Apache-2.0

include(${BASEDIR}/projects/sources.cmake)

add_library(fpl_app OBJECT ${APP_SRCS})
target_include_directories(fpl_app PUBLIC ${APP_INCS})
target_compile_definitions(fpl_app PUBLIC ${APP_DEFS})

add_subdirectory(external/libmcu)
add_subdirectory(external/pble)
add_subdirectory(external/pwifi)
add_subdirectory(external/pl4)
add_subdirectory(external/pmqtt)

target_link_libraries(fpl_app PUBLIC
	libmcu
	pble
	pwifi
	pl4
	pmqtt
)

target_compile_definitions(libmcu PRIVATE
	METRICS_USER_DEFINES=\"${PROJECT_SOURCE_DIR}/include/metrics.def\"
	_POSIX_THREADS
	_POSIX_C_SOURCE=200809L
)
