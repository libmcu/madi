# SPDX-License-Identifier: MIT

set(MCUMGR_ROOT ${PROJECT_SOURCE_DIR}/external/mynewt-mcumgr)

add_library(mcumgr STATIC
	${MCUMGR_ROOT}/port/libmcu_cbor/cbor_port.c
	${MCUMGR_ROOT}/mgmt/src/mgmt.c
	${MCUMGR_ROOT}/smp/src/smp.c
	${MCUMGR_ROOT}/cborattr/src/cborattr.c
	${MCUMGR_ROOT}/cmd/img_mgmt/src/img_mgmt.c
	${MCUMGR_ROOT}/cmd/img_mgmt/src/img_mgmt_state.c
	${MCUMGR_ROOT}/cmd/img_mgmt/src/img_mgmt_util.c
	${MCUMGR_ROOT}/util/src/mcumgr_util.c
	${MCUMGR_ROOT}/cmd/os_mgmt/src/os_mgmt.c
)

target_include_directories(mcumgr PUBLIC
	${MCUMGR_ROOT}/port/libmcu_cbor
	${MCUMGR_ROOT}/mgmt/include
	${MCUMGR_ROOT}/smp/include
	${MCUMGR_ROOT}/cborattr/include
	${MCUMGR_ROOT}/cmd/img_mgmt/include
	${MCUMGR_ROOT}/cmd/os_mgmt/include
	${MCUMGR_ROOT}/util/include
)

target_link_libraries(mcumgr PUBLIC cbor)

target_compile_options(mcumgr PUBLIC
	"SHELL:-include ${CMAKE_CURRENT_LIST_DIR}/mcumgr_config.h"
)
