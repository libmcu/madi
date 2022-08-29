include(${BASEDIR}/projects/version.cmake)

set(APP_SRCS
	${BASEDIR}/src/main.c
)

set(APP_INCS
	${BASEDIR}/include
)

set(APP_DEFS
	BUILD_DATE=${BUILD_DATE}
	VERSION_TAG=${VERSION_TAG}
	VERSION=${VERSION}
)
