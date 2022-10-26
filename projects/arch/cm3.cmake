# SPDX-License-Identifier: Apache-2.0

include(${BASEDIR}/projects/arch/common.cmake)

add_compile_options(-mcpu=cortex-m3)
add_link_options(-mcpu=cortex-m3)
