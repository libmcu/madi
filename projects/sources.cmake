# SPDX-License-Identifier: Apache-2.0

set(fpl-src-dirs src)
foreach(dir ${fpl-src-dirs})
	file(GLOB_RECURSE fpl_${dir}_SRCS RELATIVE ${BASEDIR} ${dir}/*.c)
	file(GLOB_RECURSE fpl_${dir}_CPP_SRCS RELATIVE ${BASEDIR} ${dir}/*.cpp)
	list(APPEND FPL_SRCS ${fpl_${dir}_SRCS} ${fpl_${dir}_CPP_SRCS})
endforeach()

set(APP_SRCS ${FPL_SRCS})
set(APP_INCS ${BASEDIR}/include)
set(APP_DEFS
	${BOARD}
	BUILD_DATE=${BUILD_DATE}
	VERSION_TAG=${VERSION_TAG}
	VERSION=${VERSION}

	_POSIX_THREADS
	_POSIX_C_SOURCE=200809L
)
