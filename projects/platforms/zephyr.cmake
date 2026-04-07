# --- zephyr_require_module(NAME <name> HINTS <dirs...> [REQUIRED]) ---
function(zephyr_require_module)
	set(options REQUIRED)
	set(oneValueArgs NAME)
	set(multiValueArgs HINTS)
	cmake_parse_arguments(ZRM "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(NOT ZRM_NAME)
		message(FATAL_ERROR "zephyr_require_module: NAME is required")
	endif()

	set(_candidates ${ZRM_HINTS}
		"$ENV{ZEPHYR_BASE}/../modules/${ZRM_NAME}"
		"$ENV{ZEPHYR_BASE}/../modules/crypto/${ZRM_NAME}"
		"$ENV{ZEPHYR_BASE}/../modules/fs/${ZRM_NAME}"
		"$ENV{ZEPHYR_BASE}/../modules/hal/${ZRM_NAME}"
		"$ENV{ZEPHYR_BASE}/../modules/debug/${ZRM_NAME}"
		"$ENV{ZEPHYR_BASE}/../bootloader/${ZRM_NAME}"
	)

	set(_found FALSE)
	foreach(_p IN LISTS _candidates)
		if(EXISTS "${_p}/zephyr/module.yml")
			list(APPEND ZEPHYR_EXTRA_MODULES "${_p}")
			set(ZEPHYR_EXTRA_MODULES "${ZEPHYR_EXTRA_MODULES}" PARENT_SCOPE)
			message(STATUS "Zephyr module '${ZRM_NAME}' found at: ${_p}")
			set(_found TRUE)
			break()
		endif()
	endforeach()

	if(NOT _found)
		if(ZRM_REQUIRED)
			message(FATAL_ERROR "Zephyr module '${ZRM_NAME}' not found. Checked: ${_candidates}")
		else()
			message(WARNING "Zephyr module '${ZRM_NAME}' not found. Skipping.")
		endif()
	endif()
endfunction()

set(BOARD ${TARGET_PLATFORM})
set(CONF_FILE ${CMAKE_CURRENT_LIST_DIR}/../../ports/zephyr/prj.conf)
list(APPEND BOARD_ROOT ${CMAKE_CURRENT_LIST_DIR}/../../ports/zephyr)

# MCUboot signing key (must be set before find_package(Zephyr))
set(MCUBOOT_SIGNATURE_KEY_FILE
	"${CMAKE_CURRENT_LIST_DIR}/../../secrets/dfu_signing_dev.key"
	CACHE STRING "MCUboot signing key" FORCE)

# Generate overlay file for freestanding app
set(mcuboot_overlay "${CMAKE_BINARY_DIR}/mcuboot_signing.conf")
file(WRITE "${mcuboot_overlay}"
	"# Auto-generated MCUboot signing configuration\n"
	"CONFIG_MCUBOOT_SIGNATURE_KEY_FILE=\"${MCUBOOT_SIGNATURE_KEY_FILE}\"\n"
)
list(APPEND OVERLAY_CONFIG "${mcuboot_overlay}")

message(STATUS "MCUboot signing key: ${MCUBOOT_SIGNATURE_KEY_FILE}")

# Extract version from version.h for MCUboot image signing
set(VERSION_H "${CMAKE_CURRENT_LIST_DIR}/../../include/version.h")
# Reconfigure automatically when version.h changes
set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS "${VERSION_H}")
if(EXISTS "${VERSION_H}")
	file(READ "${VERSION_H}" VERSION_H_CONTENTS)
	string(REGEX MATCH "#define[ \t]+APP_VERSION_MAJOR[ \t]+([0-9]+)" _ "${VERSION_H_CONTENTS}")
	set(APP_VERSION_MAJOR ${CMAKE_MATCH_1})
	string(REGEX MATCH "#define[ \t]+APP_VERSION_MINOR[ \t]+([0-9]+)" _ "${VERSION_H_CONTENTS}")
	set(APP_VERSION_MINOR ${CMAKE_MATCH_1})
	string(REGEX MATCH "#define[ \t]+APP_VERSION_PATCH[ \t]+([0-9]+)" _ "${VERSION_H_CONTENTS}")
	set(APP_VERSION_PATCH ${CMAKE_MATCH_1})
	set(APP_VERSION "${APP_VERSION_MAJOR}.${APP_VERSION_MINOR}.${APP_VERSION_PATCH}")

	# Generate overlay config with version for MCUboot signing
	set(version_overlay "${CMAKE_BINARY_DIR}/version_overlay.conf")
	file(WRITE "${version_overlay}"
		"# Auto-generated version from version.h\n"
		"CONFIG_MCUBOOT_IMGTOOL_SIGN_VERSION=\"${APP_VERSION}\"\n"
	)
	list(APPEND OVERLAY_CONFIG "${version_overlay}")

	message(STATUS "Application version from version.h: ${APP_VERSION}")
else()
	message(FATAL_ERROR "version.h not found: ${VERSION_H}")
endif()

zephyr_require_module(NAME mbedtls REQUIRED)
zephyr_require_module(NAME littlefs REQUIRED)
zephyr_require_module(NAME segger REQUIRED)
zephyr_require_module(NAME mcuboot REQUIRED)

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})
