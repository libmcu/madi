# SPDX-License-Identifier: Apache-2.0

add_executable(${PROJECT_NAME} ${APP_SRCS})
target_include_directories(${PROJECT_NAME} PRIVATE ${APP_INCS})
target_compile_definitions(${PROJECT_NAME} PRIVATE ${APP_DEFS})
target_link_libraries(${PROJECT_NAME} PRIVATE
	libmcu
	pble
	pwifi
	pl4
	pmqtt

	-Wl,--cref
	-Wl,--Map=\"${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map\"
)
