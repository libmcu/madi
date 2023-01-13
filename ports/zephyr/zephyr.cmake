# SPDX-License-Identifier: Apache-2.0

target_sources(app PRIVATE
	${CMAKE_CURRENT_LIST_DIR}/cli.c
)

target_link_libraries(app PUBLIC fpl_app)

target_link_libraries(libmcu PUBLIC zephyr_interface)
target_link_libraries(pble PUBLIC zephyr_interface)
target_link_libraries(pwifi PUBLIC zephyr_interface)
target_link_libraries(pl4 PUBLIC zephyr_interface)
target_link_libraries(pmqtt PUBLIC zephyr_interface)
