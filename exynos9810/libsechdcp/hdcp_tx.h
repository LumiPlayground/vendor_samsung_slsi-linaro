/*
 * HDCP Tx API
 *
 * Copyright (C) 2013 Samsung Electronics Co., LTD
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __HDCP_TX_H__
#define __HDCP_TX_H__

#include <inttypes.h>
#include "hdcp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t HDCPTxSessionHandle_t;
typedef struct HDCPTxLinkHandle {
	HDCPTxSessionHandle_t ss_handle;
	uint32_t lk_id;
} HDCPTxLinkHandle_t;

typedef struct HDCPTxElstream {
	uint8_t type;
	uint16_t stream_pid;
	uint32_t stream_ctr;
} HDCPTxElstream_t;

enum HDCPResult HDCP_TX_initialize(void);
enum HDCPResult HDCP_TX_terminate(void);
enum HDCPResult HDCP_TX_Session_open(HDCPTxSessionHandle_t *ss_handle);
enum HDCPResult HDCP_TX_Session_close(HDCPTxSessionHandle_t ss_handle);
enum HDCPResult HDCP_TX_Link_open(HDCPTxSessionHandle_t ss_handle, char *ip, uint32_t port, HDCPTxLinkHandle_t *lk_handle);
enum HDCPResult HDCP_TX_Link_close(HDCPTxLinkHandle_t lk_handle);
enum HDCPResult HDCP_TX_Link_authenticate(HDCPTxLinkHandle_t lk_handle);
enum HDCPResult HDCP_TX_Link_encrypt(HDCPTxLinkHandle_t lk_handle,
			uint8_t *pes_private,
			uint32_t priv_len,
			uint8_t *str_ctr,
			uint8_t *input_ctr,
			uint64_t input,
			uint32_t input_len,
			uint64_t output,
			uint32_t *output_len,
			uint32_t inFD,
			uint32_t outFD);

enum HDCPResult HDCP_TX_Link_stream_manage(HDCPTxLinkHandle_t lk_handle,
	uint32_t num, struct HDCPTxElstream *streams);

#ifdef __cplusplus
}
#endif

#endif
