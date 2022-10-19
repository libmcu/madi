# SPDX-License-Identifier: Apache-2.0

set(LIBMCU_ROOT ${BASEDIR}/external/libmcu)
list(APPEND LIBMCU_MODULES cli logging)
include(${LIBMCU_ROOT}/projects/modules.cmake)

set(fpl-src-dirs src stubs)
foreach(dir ${fpl-src-dirs})
	file(GLOB_RECURSE fpl_${dir}_SRCS RELATIVE ${BASEDIR} ${dir}/*.c)
	list(APPEND FPL_SRCS ${fpl_${dir}_SRCS})
endforeach()

set(APP_SRCS
	${FPL_SRCS}
	${LIBMCU_MODULES_SRCS}
)

set(APP_INCS
	${BASEDIR}/include
	${LIBMCU_MODULES_INCS}
)

set(APP_DEFS
	${BOARD}
	BUILD_DATE=${BUILD_DATE}
	VERSION_TAG=${VERSION_TAG}
	VERSION=${VERSION}
)
