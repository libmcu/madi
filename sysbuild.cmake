# SPDX-License-Identifier: MIT
#
# Sysbuild CMake configuration for madi-nrf52.
#
# This file is invoked by NCS sysbuild for each image in the build.
# ${image} is the current image name; ${APP_DIR} is the absolute path
# to the application directory (set by ExternalZephyrProject_Add).
#
# Using ${APP_DIR} for key paths avoids the WEST_TOPDIR-relative resolution
# that would otherwise require the project to live inside the NCS workspace.

# Embed our public key into MCUboot so it can verify signed app images.
# SB_CONFIG_BOOT_SIGNATURE_KEY_FILE would default to NCS's root-ec-p256.pem;
# override it here so MCUboot embeds our project's key instead.
set_config_string(mcuboot CONFIG_BOOT_SIGNATURE_KEY_FILE
    "${APP_DIR}/secrets/dfu_signing_dev.key")

# MAIN_image_default.cmake (Zephyr sysbuild) runs as part of IMAGE_CONF_SCRIPT
# and propagates SB_CONFIG_BOOT_SIGNATURE_KEY_FILE (which defaults to NCS's
# root-ec-p256.pem) to the app image as CONFIG_MCUBOOT_SIGNATURE_KEY_FILE.
# Because IMAGE_CONF_SCRIPT runs AFTER this file, NCS's default overrides the
# key we set above for the app image.
#
# Append our own script so it runs last and wins.
set_property(TARGET ${image} APPEND PROPERTY IMAGE_CONF_SCRIPT
    "${APP_DIR}/sysbuild_signing.cmake")

# Provide a static Partition Manager layout for sysbuild to avoid
# dynamic partition placement warnings when MCUboot is enabled.
set(PM_STATIC_YML_FILE "${APP_DIR}/pm_static_madi_nrf52840.yml" CACHE INTERNAL "")
