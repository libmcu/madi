# SPDX-License-Identifier: MIT

# Keep ESP-IDF app descriptor version in sync with include/version.h
# so mcumgr image list shows the real firmware version.
set(APP_VERSION_HEADER "${CMAKE_SOURCE_DIR}/include/version.h")
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${APP_VERSION_HEADER}")
if (EXISTS "${APP_VERSION_HEADER}")
	file(READ "${APP_VERSION_HEADER}" VERSION_H_CONTENTS)

	string(REGEX MATCH "#define[ \t]+APP_VERSION_MAJOR[ \t]+([0-9]+)" _ "${VERSION_H_CONTENTS}")
	set(APP_VERSION_MAJOR ${CMAKE_MATCH_1})
	string(REGEX MATCH "#define[ \t]+APP_VERSION_MINOR[ \t]+([0-9]+)" _ "${VERSION_H_CONTENTS}")
	set(APP_VERSION_MINOR ${CMAKE_MATCH_1})
	string(REGEX MATCH "#define[ \t]+APP_VERSION_PATCH[ \t]+([0-9]+)" _ "${VERSION_H_CONTENTS}")
	set(APP_VERSION_PATCH ${CMAKE_MATCH_1})

	if (NOT "${APP_VERSION_MAJOR}" STREQUAL "" AND
	    NOT "${APP_VERSION_MINOR}" STREQUAL "" AND
	    NOT "${APP_VERSION_PATCH}" STREQUAL "")
		set(PROJECT_VER "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}")
		message(STATUS "Setting PROJECT_VER from version.h: ${PROJECT_VER}")
	endif()
endif()

set(CMAKE_TOOLCHAIN_FILE $ENV{IDF_PATH}/tools/cmake/toolchain-${IDF_TARGET}.cmake)
