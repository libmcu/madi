/*
 * SPDX-FileCopyrightText: 2023 Kyunghwan Kwon <k@libmcu.org>
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "httpsrv.h"

#include <string.h>

#include "libmcu/logging.h"
#include "lwip/apps/httpd.h"

static const char *ssi_tags[] = {
	"AjaxCall",
	"Result",
};

static uint16_t handle_ssi_ajax(char *buf, int buflen,
		uint16_t current_part, uint16_t *next_part);
static uint16_t handle_ssi_result(char *buf, int buflen,
		uint16_t current_part, uint16_t *next_part);
static uint16_t (*ssi_handlers[])(char *buf, int buflen,
		uint16_t current_part, uint16_t *next_part) = {
	handle_ssi_ajax,
	handle_ssi_result,
};

static uint16_t handle_ssi_ajax(char *buf, int buflen,
		uint16_t current_part, uint16_t *next_part)
{
	(void)current_part;
	(void)next_part;
	(void)buf;
	(void)buflen;
	return 0;
}

static uint16_t handle_ssi_result(char *buf, int buflen,
		uint16_t current_part, uint16_t *next_part)
{
	(void)current_part;
	(void)next_part;
	(void)buf;
	(void)buflen;
	return 0;
}

static uint16_t ssi_handler(int idx, char *buf, int buflen,
		uint16_t current_part, uint16_t *next_part)
{
	uint16_t len = 0;

	if (idx < (int)(sizeof(ssi_tags) / sizeof(ssi_tags[0]))) {
		len = (*ssi_handlers[idx])(buf, buflen,
				current_part, next_part);
	}

	return len;
}

err_t httpd_post_begin(void *connection, const char *uri,
		const char *http_request, u16_t http_request_len,
		int content_len, char *response_uri, u16_t response_uri_len,
		u8_t *post_auto_wnd)
{
	if (strcmp(uri, "/dfu") != 0) {
		return ERR_VAL;
	}

	(void)connection;
	(void)http_request_len;
	(void)response_uri;
	(void)response_uri_len;
	(void)post_auto_wnd;
	(void)http_request;
	debug("REQ %.*s", http_request_len, http_request);
	debug("Post %s | %s, %d", uri, response_uri, content_len);

	return ERR_OK;
}

err_t httpd_post_receive_data(void *connection, struct pbuf *p)
{
	(void)connection;
	debug("Recv %lu %.*s", p->tot_len, p->tot_len, p->payload);
	pbuf_free(p);
	return ERR_OK;
}

void httpd_post_finished(void *connection,
		char *response_uri, u16_t response_uri_len)
{
	(void)connection;
	snprintf(response_uri, response_uri_len, "/index.shtml");
}

int httpsrv_init(void)
{
	httpd_init();
	http_set_ssi_handler(ssi_handler,
			ssi_tags, sizeof(ssi_tags) / sizeof(ssi_tags[0]));
	return 0;
}
