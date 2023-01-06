# SPDX-License-Identifier: Apache-2.0

include(${BASEDIR}/projects/sources.cmake)

add_library(fpl_app OBJECT ${APP_SRCS})
target_include_directories(fpl_app PUBLIC ${APP_INCS})
target_compile_definitions(fpl_app PUBLIC ${APP_DEFS})

add_subdirectory(external/libmcu)
add_subdirectory(external/pble)
add_subdirectory(external/pwifi)

target_link_libraries(fpl_app libmcu)
target_link_libraries(fpl_app pble)
target_link_libraries(fpl_app pwifi)

target_compile_definitions(libmcu PRIVATE
	METRICS_USER_DEFINES=\"${PROJECT_SOURCE_DIR}/include/metrics.def\"
	_POSIX_THREADS
	_POSIX_C_SOURCE=200809L
)
