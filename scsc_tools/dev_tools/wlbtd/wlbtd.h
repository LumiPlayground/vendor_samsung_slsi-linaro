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
#ifndef __SCSC_WLBTD_H
#define __SCSC_WLBTD_H
/**
 * Attributes and commands have to be the same as in kernelspace, so you might
 * want to move these enums to a .h and just #include that from both files.
 */
enum attributes {
	/* must be first */
	ATTR_UNSPEC,

	ATTR_STR,
	ATTR_INT,
	ATTR_PATH,
	ATTR_CONTENT,
	ATTR_INT8,

	/* This must be last! */
	__ATTR_MAX,
};

enum events {
	/* must be first */
	EVENT_UNSPEC,

	EVENT_SCSC,
	EVENT_SYSTEM_PROPERTY,
	EVENT_WRITE_FILE,
	EVENT_SABLE,
	EVENT_CHIPSET_LOGGING,

	/* This must be last! */
	__EVENT_MAX,
};

void parse_memdump_file_wlbtd(void);
int wlbtd_popen(const char *command);
const char *get_trigger_str(int trigger_int);
void send_response_to_kernel(enum events e, const char *msg_str, int status);

#if __linux__ && !__ANDROID__
static const char* log_collection_script = "/usr/bin/mx_log_collection_x86.sh";
#else
static const char* log_collection_script = "/vendor/bin/mx_log_collection.sh";
#endif

#endif
