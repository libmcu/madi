# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/cli.c
	${APP_SRCS}
)
target_include_directories(app PRIVATE ${APP_INCS})
target_compile_definitions(app PRIVATE ${APP_DEFS})

target_link_libraries(app PRIVATE
	libmcu
	pble
	pwifi
	pl4
	pmqtt
)

target_link_libraries(libmcu PUBLIC zephyr_interface)
target_link_libraries(pble PUBLIC zephyr_interface)
target_link_libraries(pwifi PUBLIC zephyr_interface)
target_link_libraries(pl4 PUBLIC zephyr_interface)
target_link_libraries(pmqtt PUBLIC zephyr_interface)
