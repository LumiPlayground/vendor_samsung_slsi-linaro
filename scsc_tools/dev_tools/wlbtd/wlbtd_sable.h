/*
 * Copyright (C) 2021 Samsung Electronics Co. Ltd.
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
 */
#ifndef __WLBTD_SABLE
#define __WLBTD_SABLE

#include <pthread.h>
#include <inttypes.h>
#define MAX_LOG_STR_KERNEL 128
/*
 * Index acts as slot for non fw panic sable trigger request
 * and fw panic sable trigger request. To be expanded as
 * additional subsystems will be added
 */
typedef enum __sable_req_type {
	SABLE_FW_PANIC_REQ = 0,
	SABLE_NON_FW_PANIC_REQ,
	SABLE_LAST_REQ,
} sable_req_type;

typedef struct __sable_req {
	uint16_t error_code;
	uint8_t trigger_uint8;
	const char *trigger_str;
	sable_req_type type;
} sable_req;

typedef struct __wlbtd_sableq {
	sable_req *pos[SABLE_LAST_REQ];
	pthread_cond_t cond_var = PTHREAD_COND_INITIALIZER;
	pthread_mutex_t cond_var_lock = PTHREAD_MUTEX_INITIALIZER;
} wlbtd_sableq;

enum scsc_log_reason {
	SCSC_LOG_UNKNOWN = 0,
	SCSC_LOG_FW_PANIC,
	SCSC_LOG_USER,
	SCSC_LOG_FW,
	SCSC_LOG_DUMPSTATE,
	SCSC_LOG_HOST_WLAN,
	SCSC_LOG_HOST_BT,
	SCSC_LOG_HOST_COMMON,
	/* Add others */
};

enum scsc_wlbtd_response_codes {
	/* NOTE: keep the enum in sync with driver */
	/* parse failed */
	SCSC_WLBTD_ERR_PARSE_FAILED,

	/* fw_panic trigger */
	SCSC_WLBTD_FW_PANIC_TAR_GENERATED,
	SCSC_WLBTD_FW_PANIC_ERR_SCRIPT_FILE_NOT_FOUND,
	SCSC_WLBTD_FW_PANIC_ERR_NO_DEV,
	SCSC_WLBTD_FW_PANIC_ERR_MMAP,
	SCSC_WLBTD_FW_PANIC_ERR_SABLE_FILE,
	SCSC_WLBTD_FW_PANIC_ERR_TAR,

	/* other triggers */
	SCSC_WLBTD_OTHER_SBL_GENERATED,
	SCSC_WLBTD_OTHER_TAR_GENERATED,
	SCSC_WLBTD_OTHER_ERR_SCRIPT_FILE_NOT_FOUND,
	SCSC_WLBTD_OTHER_ERR_NO_DEV,
	SCSC_WLBTD_OTHER_ERR_MMAP,
	SCSC_WLBTD_OTHER_ERR_SABLE_FILE,
	SCSC_WLBTD_OTHER_ERR_TAR,
	SCSC_WLBTD_OTHER_IGNORE_TRIGGER,
};

void wlbtd_sable_init();
void wlbtd_sable_destroy();
void wlbtd_process_sable_req();
int wlbtd_sable_queue_req(sable_req *req, char *logmsg, size_t logmsg_size);
#endif
