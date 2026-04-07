/* SPDX-License-Identifier: MIT */

#ifndef MCUMGR_CONFIG_H
#define MCUMGR_CONFIG_H

/* Claim the library config headers' include guards so they are skipped
 * when included source-relatively (e.g. from img_mgmt.h). */
#define H_IMG_MGMT_CONFIG_
#define H_OS_MGMT_CONFIG_

/* img_mgmt */
#define IMG_MGMT_UPDATABLE_IMAGE_NUMBER 1
#define IMG_MGMT_FRUGAL_LIST            0
#define IMG_MGMT_UL_CHUNK_SIZE          512
#define IMG_MGMT_VERBOSE_ERR            0
#define IMG_MGMT_LAZY_ERASE             0
#define IMG_MGMT_DUMMY_HDR              0
#define IMG_MGMT_BOOT_CURR_SLOT         0

/* os_mgmt */
#define OS_MGMT_RESET_MS                200
#define OS_MGMT_TASKSTAT                0
#define OS_MGMT_ECHO                    1
#define OS_MGMT_DATETIME                0
#define DATETIME_BUFSIZE                33

/* cborattr */
#ifndef CBORATTR_MAX_SIZE
#define CBORATTR_MAX_SIZE               512
#endif

/* mynewt compat */
#ifndef MYNEWT_VAL
#define MYNEWT_VAL(x)                   0
#endif

#endif /* MCUMGR_CONFIG_H */
