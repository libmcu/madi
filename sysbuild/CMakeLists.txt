# Copyright (c) 2026 libmcu
#
# SPDX-License-Identifier: MIT

cmake_minimum_required(VERSION 3.20)

find_package(Sysbuild REQUIRED HINTS $ENV{ZEPHYR_BASE} COMPONENTS
  extensions
  sysbuild_extensions
  python
  west
  yaml
  sysbuild_root
  zephyr_module
  boards
  shields
  hwm_v2
  sysbuild_snippets
  sysbuild_kconfig
  native_simulator_sb_extensions
)

project(sysbuild LANGUAGES NONE)

get_filename_component(APP_DIR ${APP_DIR} ABSOLUTE)
set(DEFAULT_IMAGE "app")

function(sysbuild_info_image images)
  set(info_image_list)
  foreach(image ${images})
    ExternalProject_Get_Property(${image} SOURCE_DIR)
    get_target_property(type ${image} APP_TYPE)
    if(type)
      list(APPEND info_image_list MAP "name: ${image}, source-dir: ${SOURCE_DIR}, type: ${type}")
    else()
      list(APPEND info_image_list MAP "name: ${image}, source-dir: ${SOURCE_DIR}")
    endif()
  endforeach()
  build_info(images VALUE ${info_image_list})
  yaml_save(NAME build_info)
endfunction()

# Use the default sysbuild images directory but override DEFAULT_IMAGE first.
sysbuild_add_subdirectory(${sysbuild_toplevel_SOURCE_DIR}/images sysbuild/images)

get_property(IMAGES GLOBAL PROPERTY sysbuild_images)
sysbuild_module_call(PRE_CMAKE MODULES ${SYSBUILD_MODULE_NAMES} IMAGES ${IMAGES})
sysbuild_images_order(IMAGES_CONFIGURATION_ORDER CONFIGURE IMAGES ${IMAGES})

sysbuild_info_image("${IMAGES}")

foreach(image ${IMAGES_CONFIGURATION_ORDER})
  sysbuild_module_call(PRE_IMAGE_CMAKE MODULES ${SYSBUILD_MODULE_NAMES} IMAGES ${IMAGES} IMAGE ${image})
  ExternalZephyrProject_Cmake(APPLICATION ${image})
  sysbuild_module_call(POST_IMAGE_CMAKE MODULES ${SYSBUILD_MODULE_NAMES} IMAGES ${IMAGES} IMAGE ${image})
endforeach()

sysbuild_module_call(POST_CMAKE MODULES ${SYSBUILD_MODULE_NAMES} IMAGES ${IMAGES})

sysbuild_module_call(PRE_DOMAINS MODULES ${SYSBUILD_MODULE_NAMES} IMAGES ${IMAGES})
include(${sysbuild_toplevel_SOURCE_DIR}/cmake/domains.cmake)
sysbuild_module_call(POST_DOMAINS MODULES ${SYSBUILD_MODULE_NAMES} IMAGES ${IMAGES})
