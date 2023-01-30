# SPDX-License-Identifier: Apache-2.0

add_executable(${PROJECT_NAME} ${APP_SRCS})
target_include_directories(${PROJECT_NAME} PUBLIC ${APP_INCS})
target_compile_definitions(${PROJECT_NAME} PUBLIC ${APP_DEFS})
target_link_libraries(${PROJECT_NAME} PUBLIC
	libmcu
	pble
	pwifi
	pl4
	pmqtt
	bq25180

	-Wl,--cref
	-Wl,--Map=\"${CMAKE_BINARY_DIR}/${PROJECT_NAME}.map\"
)
