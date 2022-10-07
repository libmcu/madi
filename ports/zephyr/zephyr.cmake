# SPDX-License-Identifier: Apache-2.0

zephyr_library_sources_ifdef(CONFIG_WIFI ${CMAKE_SOURCE_DIR}/drivers/wifi/zephyr.c)

zephyr_include_directories(${APP_INCS})
zephyr_compile_definitions(${APP_DEFS})
target_sources(app PRIVATE
	${APP_SRCS}
	${CMAKE_CURRENT_LIST_DIR}/cli.c
)
