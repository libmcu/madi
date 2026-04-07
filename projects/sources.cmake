# SPDX-License-Identifier: MIT

set(src-dirs
	src
)

foreach(dir ${src-dirs})
	file(GLOB_RECURSE ${dir}_SRCS RELATIVE ${CMAKE_SOURCE_DIR} ${dir}/*.c)
	file(GLOB_RECURSE ${dir}_CPP_SRCS RELATIVE ${CMAKE_SOURCE_DIR} ${dir}/*.cpp)
	list(APPEND SRCS_TMP ${${dir}_SRCS} ${${dir}_CPP_SRCS})
endforeach()

set(APP_SRCS
	${SRCS_TMP}
)
set(APP_INCS
	${CMAKE_SOURCE_DIR}/include
	${CMAKE_SOURCE_DIR}/external/libmcu/ports/armcm/include
)
set(APP_DEFS
	${PROJECT}
	TARGET_PLATFORM_${TARGET_PLATFORM}

	BUILD_DATE=${BUILD_DATE}
	VERSION_TAG=${VERSION_TAG}
	VERSION=${VERSION}
)
