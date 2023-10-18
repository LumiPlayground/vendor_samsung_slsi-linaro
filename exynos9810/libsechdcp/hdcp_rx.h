/*
 * HDCP Rx API
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

#ifndef __HDCP_RX_H__
#define __HDCP_RX_H__

#include <inttypes.h>
#include "hdcp_common.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct HDCPRxLinkHandle {
	void *lk_id;
} HDCPRxLinkHandle_t;

enum HDCPResult HDCP_RX_initialize(void);
enum HDCPResult HDCP_RX_terminate(void);
enum HDCPResult HDCP_RX_Link_open(uint32_t port, HDCPRxLinkHandle_t *lk_handle);
enum HDCPResult HDCP_RX_Link_close(HDCPRxLinkHandle_t lk_handle);
enum HDCPResult HDCP_RX_Link_authenticate(HDCPRxLinkHandle_t lk_handle);
enum HDCPResult HDCP_RX_Link_decrypt(HDCPRxLinkHandle_t lk_handle,
	uint8_t *pes_private, uint32_t priv_len,
	uint8_t *input, uint32_t input_len,
	uint8_t *output, uint32_t *output_len);

#ifdef __cplusplus
}
#endif
#endif
