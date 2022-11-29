# SPDX-License-Identifier: Apache-2.0

set(LIBMCU_ROOT ${BASEDIR}/external/libmcu)
list(APPEND LIBMCU_MODULES cli logging trace)
include(${LIBMCU_ROOT}/projects/modules.cmake)

set(fpl-src-dirs src stubs)
foreach(dir ${fpl-src-dirs})
	file(GLOB_RECURSE fpl_${dir}_SRCS RELATIVE ${BASEDIR} ${dir}/*.c)
	file(GLOB_RECURSE fpl_${dir}_CPP_SRCS RELATIVE ${BASEDIR} ${dir}/*.cpp)
	list(APPEND FPL_SRCS ${fpl_${dir}_SRCS} ${fpl_${dir}_CPP_SRCS})
endforeach()

set(APP_SRCS
	${FPL_SRCS}
	${LIBMCU_MODULES_SRCS}
	${BASEDIR}/drivers/ble/common.c
)

set(APP_INCS
	${BASEDIR}/include
	${BASEDIR}/drivers/include
	${LIBMCU_MODULES_INCS}
)

set(APP_DEFS
	${BOARD}
	BUILD_DATE=${BUILD_DATE}
	VERSION_TAG=${VERSION_TAG}
	VERSION=${VERSION}
)
