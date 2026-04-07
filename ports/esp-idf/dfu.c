/*
 * SPDX-FileCopyrightText: 2023 권경환 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: MIT
 */

#include "libmcu/dfu.h"

#include <stdlib.h>
#include <string.h>

#include "libmcu/metrics.h"
#include "mbedtls/sha256.h"
#include "esp_ota_ops.h"

#include "logging.h"

#if !defined(FLASH_SECTOR_SIZE)
#define FLASH_SECTOR_SIZE	4096U
#endif

#if !defined(MIN)
#define MIN(a, b)		((a) > (b)? (b) : (a))
#endif

struct dfu {
	struct dfu_image_header header;

	esp_ota_handle_t ota_handle;
	const esp_partition_t *slot;
	uint8_t *buf;
	size_t bufsize;
};

static bool is_valid_header(const struct dfu_image_header *header)
{
	return header->magic == 0xC0DEu && header->type == DFU_TYPE_APP;
}

static bool verify_digest(const struct dfu *dfu)
{
	if (dfu->header.datasize == 0 ||
			dfu->header.datasize > dfu->slot->size) {
		return false;
	}

	mbedtls_sha256_context sha;
	mbedtls_sha256_init(&sha);
	size_t chunk = 0;

	if (mbedtls_sha256_starts(&sha, 0/*sha256*/) != 0) {
		goto out_free;
	}

	for (size_t i = 0; i < dfu->header.datasize; i += chunk) {
		chunk = MIN(dfu->header.datasize - i, dfu->bufsize);

		if (esp_partition_read(dfu->slot, i, dfu->buf, chunk) != ESP_OK
				|| mbedtls_sha256_update(&sha,
						dfu->buf, chunk) != 0) {
			goto out_free;
		}
	}

	uint8_t digest[32] = { 0, };
	mbedtls_sha256_finish(&sha, digest);
out_free:
	mbedtls_sha256_free(&sha);

	if (memcmp(dfu->header.signature, digest, sizeof(digest)) == 0) {
		return true;
	}

	return false;
}

bool dfu_is_valid_header(const struct dfu_image_header *header)
{
	return is_valid_header(header);
}

dfu_error_t dfu_write(struct dfu *dfu, uint32_t offset,
		const void *data, size_t datasize)
{
	if (esp_ota_write(dfu->ota_handle, dfu->buf, datasize) != ESP_OK) {
		metrics_increase(DFUIOErrorCount);
		return DFU_ERROR_IO;
	}

	return DFU_ERROR_NONE;
}

dfu_error_t dfu_prepare(struct dfu *dfu, const struct dfu_image_header *header)
{
	memcpy(&dfu->header, header, sizeof(*header));

	if (!is_valid_header(&dfu->header)) {
		metrics_increase(DFUPrepareErrorCount);
		return DFU_ERROR_INVALID_HEADER;
	}

	/* Erases flash memory in small sectors to reduce blocking time.
	 * Instead of erasing large regions at once, this method divides the
	 * operation into smaller sector-sized chunks. This minimizes blocking
	 * time from seconds to milliseconds. Note that the erase operation
	 * must always align with sector boundaries. Using arbitrary sizes
	 * may lead to overlapping regions, causing data corruption or reduced
	 * efficiency. */
	if (esp_ota_begin(dfu->slot, OTA_WITH_SEQUENTIAL_WRITES,
			&dfu->ota_handle) != ESP_OK) {
		metrics_increase(DFUPrepareErrorCount);
		return DFU_ERROR_INVALID_SLOT;
	}

	return DFU_ERROR_NONE;
}

dfu_error_t dfu_abort(struct dfu *dfu)
{
	esp_ota_abort(dfu->ota_handle);
	return DFU_ERROR_NONE;
}

dfu_error_t dfu_finish(struct dfu *dfu)
{
	esp_err_t err = esp_ota_end(dfu->ota_handle);

	if (err != ESP_OK || !verify_digest(dfu)) {
		metrics_increase(DFUFinishErrorCount);
		error("finalizing: %d", err);
		return DFU_ERROR_INVALID_IMAGE;
	}

	if ((err = esp_ota_set_boot_partition(dfu->slot)) != ESP_OK) {
		metrics_increase(DFUCommitErrorCount);
		error("set boot partition: %d", err);
		return DFU_ERROR_SLOT_UPDATE_FAIL;
	}

	metrics_increase(DFUSuccessCount);
	return DFU_ERROR_NONE;
}

dfu_error_t dfu_accept(void)
{
	if (esp_ota_mark_app_valid_cancel_rollback() == ESP_OK) {
		return DFU_ERROR_NONE;
	}
	error("Failed to mark app valid and cancel rollback.");
	return DFU_ERROR_SLOT_UPDATE_FAIL;
}

dfu_error_t dfu_reject_and_rollback(void)
{
	if (esp_ota_mark_app_invalid_rollback_and_reboot() == ESP_OK) {
		return DFU_ERROR_NONE;
	}
	error("Failed to mark app invalid and rollback.");
	return DFU_ERROR_SLOT_UPDATE_FAIL;
}

bool dfu_is_pending_verify(void)
{
	const esp_partition_t *running = esp_ota_get_running_partition();
	esp_ota_img_states_t s;
	esp_err_t r = esp_ota_get_state_partition(running, &s);
	if (r == ESP_OK && s == ESP_OTA_IMG_PENDING_VERIFY) {
		return true;
	}
	debug("%d, %d", r, s);
	return false;
}

bool dfu_selftest(void)
{
	/* TODO: Implement self-test logic here.
	 * This function should perform necessary checks to ensure the
	 * firmware is functioning correctly after an update.
	 * Return true if the self-test passes, false otherwise. */
	return true;
}

struct dfu *dfu_new(size_t data_block_size)
{
	metrics_increase(DFURequestCount);
	struct dfu *p = (struct dfu *)calloc(1, sizeof(struct dfu));

	if (p) {
		if ((p->buf = (uint8_t *)malloc(data_block_size)) == NULL) {
			free(p);
			return NULL;
		}

		p->slot = esp_ota_get_next_update_partition(NULL);
		p->bufsize = data_block_size;
	}

	return p;
}

void dfu_delete(struct dfu *dfu)
{
	free(dfu->buf);
	free(dfu);
}
