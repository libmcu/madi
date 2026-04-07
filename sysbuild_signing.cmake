# SPDX-License-Identifier: MIT
#
# App image signing key override for madi-nrf52.
#
# This file is appended to IMAGE_CONF_SCRIPT by sysbuild.cmake. It runs
# AFTER MAIN_image_default.cmake (Zephyr sysbuild), which propagates
# SB_CONFIG_BOOT_SIGNATURE_KEY_FILE to the app as CONFIG_MCUBOOT_SIGNATURE_KEY_FILE.
# By running last, this override takes precedence over the NCS default key.
#
# ZCMAKE_APPLICATION is the current image variable available in IMAGE_CONF_SCRIPT
# execution context (ExternalZephyrProject_Cmake).

set_config_string(${ZCMAKE_APPLICATION} CONFIG_MCUBOOT_SIGNATURE_KEY_FILE
    "${APP_DIR}/secrets/dfu_signing_dev.key")
