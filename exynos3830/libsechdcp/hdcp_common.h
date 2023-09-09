/*
 * HDCP Common definition
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

#ifndef __HDCP_COMMON_H__
#define __HDCP_COMMON_H__

typedef enum HDCPResult {
	HDCP_SUCCESS = 0,
	HDCP_ERROR_INIT_FAILED,
	HDCP_ERROR_TERMINATE_FAILED,
	HDCP_ERROR_SESSION_OPEN_FAILED,
	HDCP_ERROR_RX_NOT_HDCP_CAPABLE,
	HDCP_ERROR_EXCHANGE_KM,
	HDCP_ERROR_LOCALITY_CHECK,
	HDCP_ERROR_EXCHANGE_KS,
	HDCP_ERROR_WAIT_RECEIVER_ID_LIST,
	HDCP_ERROR_VERIFY_RECEIVER_ID_LIST,
	HDCP_ERROR_WAIT_AKE_INIT,
	HDCP_ERROR_MALLOC_FAILED = 1000,
	HDCP_ERROR_INVALID_INPUT,
	HDCP_ERROR_INVALID_STATE,
	HDCP_ERROR_NET_UNREACHABLE,
	HDCP_ERROR_NET_CLOSED,
	HDCP_ERROR_ENCRYPTION,
	HDCP_ERROR_DECRYPTION,
	HDCP_ERROR_STREAM_MANAGE,
	HDCP_ERROR_DO_NOT_SUPPORT_YET,
	HDCP_ERROR_END
} HDCPResult;

#ifndef TRUE
#define TRUE 1
#endif

#ifndef FALSE
#define FALSE 0
#endif

#endif

