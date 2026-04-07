# SPDX-License-Identifier: MIT

if (NOT DEFINED IDF_TARGET)
	set(IDF_TARGET esp32s3)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/esp-idf_version.cmake)
