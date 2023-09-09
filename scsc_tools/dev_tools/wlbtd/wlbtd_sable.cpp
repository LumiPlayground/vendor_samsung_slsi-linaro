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
#if __ANDROID__
#include <log/log.h>
#else
#include <fcntl.h>
#include <sys/time.h>
#define ALOGD(...) do { printf(__VA_ARGS__); puts(""); } while (0)
#define ALOGE(...) do { printf(__VA_ARGS__); puts(""); } while (0)
#define ALOGI(...) do { printf(__VA_ARGS__); puts(""); } while (0)
#endif
#include <pthread.h>
#include <sched.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "wlbtd.h"
#include "wlbtd_sable.h"

#define LOG_TAG "WLBTD"
#define MAP_SIZE 16 * 1024 * 1024
#define LOG_COLLECT_DEV_ENTRY "/dev/scsc_log_collector_0"
#define LOG_IN_DRAM_DEV_ENTRY "/dev/scsc_log_in_dram_0"
#define SABLETARPATH "/data/vendor/log/wifi"

/* Log in DRAM feature for MCD */
#define WLBTD_IN_DRAM_FILENAME_SZ 128
#define WLBTD_IN_DRAM_VERSION 0
#define WLBTD_IN_DRAM_MAGIC "scscdram"
#define WLBTD_IN_DRAM_MAGIC_SIZE 8

static wlbtd_sableq sq;
static pthread_t spid;
static bool can_create_sbl_tar = true;
static int wlbtd_sable_term_signal = 0;
static pthread_mutex_t tar_cond_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t tar_cond_var = PTHREAD_COND_INITIALIZER;

struct wlbtd_in_dram_header {
	char magic[WLBTD_IN_DRAM_MAGIC_SIZE];
	unsigned int version;
	unsigned int size;
	char filename[WLBTD_IN_DRAM_FILENAME_SZ];
};

struct script_arguments {
	uint8_t trigger_uint8;
	uint16_t error_code;
};

static void sable_gen_in_progress()
{
	pthread_mutex_lock(&tar_cond_mutex);
	can_create_sbl_tar = false;
	pthread_mutex_unlock(&tar_cond_mutex);
}

static void sable_gen_complete()
{
	pthread_mutex_lock(&tar_cond_mutex);
	can_create_sbl_tar = true;
	pthread_mutex_unlock(&tar_cond_mutex);
	pthread_cond_signal(&tar_cond_var);
}

static bool sable_gen_available()
{
	bool ret;
	pthread_mutex_lock(&tar_cond_mutex);
	ret = can_create_sbl_tar;
	pthread_mutex_unlock(&tar_cond_mutex);
	return ret;
}

bool sableq_is_empty(void)
{
	int i;
	for (i = SABLE_NON_FW_PANIC_REQ; i < SABLE_LAST_REQ; i++) {
		if (sq.pos[i] != NULL) {
			return false;
		}
	}

	return true;
}

static int sableq_enqueue(sable_req *req, char *logmsg, size_t logmsg_size)
{
	int ret = 0;
	bool qstatus = sableq_is_empty();

	if ((req == NULL) || (logmsg == NULL))
		return -1;

	if (req->trigger_uint8 == SCSC_LOG_FW_PANIC)
		req->type = SABLE_FW_PANIC_REQ;
	else
		req->type = SABLE_NON_FW_PANIC_REQ;
	/*
	 * Requests are stored in queue by their type. Type here can be:
	 *
	 * 1) fw panic sable generation request
	 * 2) non-fw panic sable generation request
	 *
	 * Each of the above is stored in its own queue slot
	 * -----------------------------------------
	 * | sable fw panic  | sable non fw panic  |
	 * -----------------------------------------
	 */
	if (sq.pos[req->type] == NULL) {
		ALOGI("Adding request at pos %d\n", req->type);
		snprintf(logmsg, logmsg_size, "Adding request at pos %d\n",
			 req->type);
		sq.pos[req->type] = req;
	} else {
		ALOGI("Request for %s sable generation already queued. Ignoring request "
		      "with error code %d\n",
		      req->type == SABLE_FW_PANIC_REQ ? "FW Panic" :
							"Non FW Panic",
		      req->error_code);
		snprintf(
			logmsg, logmsg_size,
			"Request for %s sable generation already queued. Ignoring request "
			"with error code %d\n",
			req->type == SABLE_FW_PANIC_REQ ? "FW Panic" :
							  "Non FW Panic",
			req->error_code);
		ret = -1;
	}

	/*
	 * If queue was empty and we have managed to insert an item, signal
	 * conditional variable
	 */
	if (qstatus && !ret)
		pthread_cond_signal(&sq.cond_var);

	return ret;
}

/* This acts as wrapper for actual data structure used to store request */
int wlbtd_sable_queue_req(sable_req *req, char *logstr, size_t logmsg_size)
{
	int ret = 0;

	pthread_mutex_lock(&sq.cond_var_lock);
	ALOGI("Adding message to queue\n");
	ret = sableq_enqueue(req, logstr, logmsg_size);
	pthread_mutex_unlock(&sq.cond_var_lock);

	return ret;
}

/* Dequeue next element according to priority */
static sable_req *sableq_dequeue()
{
	int i;
	sable_req *req = NULL;

	for (i = SABLE_FW_PANIC_REQ; i < SABLE_LAST_REQ; i++) {
		if (sq.pos[i] != NULL) {
			ALOGI("Removing request at pos %d\n", i);
			/* Caller will free the associated memory */
			req = sq.pos[i];
			sq.pos[i] = NULL;
			return req;
		}
	}

	return NULL;
}

static int wlbtd_log_in_dram(void)
{
	int dev_fd;
	FILE *fp;
	char path[WLBTD_IN_DRAM_FILENAME_SZ];
	int sfd;
	char *src, *dest;
	int ret;
	size_t filesize;
	char *filename;
	char cmd[MAX_LOG_STR_KERNEL];
	struct wlbtd_in_dram_header *header;

	ALOGI("Checking for %s", LOG_IN_DRAM_DEV_ENTRY);
	/* Check if the kernel exposes the interface  */
	dev_fd = open(LOG_IN_DRAM_DEV_ENTRY, O_RDWR);
	if (dev_fd < 0) {
		ALOGE("failed %s to open %s\n", strerror(errno),
		      LOG_IN_DRAM_DEV_ENTRY);
		return -ENOENT;
	}

	/* Check the latest panic code created by calling shell command
         * mx_log_collection.sh last_panic */
	snprintf(cmd, MAX_LOG_STR_KERNEL, "%s %s",
		 log_collection_script, "last_panic");

	ALOGI("Check for latest FW_panic");
	/* get the last fw_panic tarball*/
	fp = popen(cmd, "r");
	if (!fp) {
		ALOGE("popen failed: %s", strerror(errno));
		close(dev_fd);
		return -ENOENT;
	}
	/* Read the output a line at a time - output it. */
	if (fgets(path, sizeof(path), fp)) {
		/* Remove end of line */
		*strchrnul(path, '\n') = '\0';
		/* Open file if exists */
		sfd = open(path, O_RDONLY);
		if (sfd < 0) {
			ALOGE("failed %s to open %s\n", strerror(errno), path);
			close(dev_fd);
			pclose(fp);
			return -ENOENT;
		}
		ret = lseek(sfd, 0, SEEK_END);
		if (ret < 0) {
			ALOGE("failed to get filesize\n");
			goto err_mmap_src;
		}
		else
			filesize = (size_t)ret;

		/* mmap the file */
		src = (char *)mmap(NULL, filesize, PROT_READ, MAP_SHARED, sfd,
				   0);
		if (src == nullptr) {
			ALOGE("mmap failed %s", strerror(errno));
			goto err_mmap_src;
		}
		ALOGI("Last fw_panic found %s size %lu", path, filesize);
	} else {
		ALOGE("fgets failed: %s", strerror(errno));
		close(dev_fd);
		pclose(fp);
		return -ENOENT;
	}

	/* Mmap the interface */
	dest = (char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
			    dev_fd, 0);
	if (dest == nullptr) {
		ALOGE("mmap failed %s in %s", strerror(errno),
		      LOG_IN_DRAM_DEV_ENTRY);
		goto err_mmap_dst;
	}

	filename = basename(path);

	ALOGI("Create wlbtd_in_dram_header");
	/* Build log in dram header */
	header = (struct wlbtd_in_dram_header *)dest;
	memcpy(header->magic, WLBTD_IN_DRAM_MAGIC, WLBTD_IN_DRAM_MAGIC_SIZE);
	header->version = WLBTD_IN_DRAM_VERSION;
	header->size = filesize;
	memcpy(header->filename, filename, WLBTD_IN_DRAM_FILENAME_SZ);

	ALOGI("Copy %s to DRAM buffer", filename);
	/* write sable file */
	memcpy(dest + sizeof(struct wlbtd_in_dram_header), src, filesize);

	/* unmap files */
	munmap(src, filesize);
	munmap(dest, MAP_SIZE);

	close(dev_fd);
	close(sfd);
	pclose(fp);

	return 0;

err_mmap_dst:
	munmap(src, filesize);
err_mmap_src:
	close(dev_fd);
	close(sfd);
	pclose(fp);
	return -ENOENT;
}

void *sable_tar_thread(void *args)
{
	struct script_arguments *arg = (struct script_arguments *)args;
	int status = -1;
	char err_code_str[5];
	char cmd[MAX_LOG_STR_KERNEL];
	struct timeval t0, t1;
	double diff;
	const char *trigger_str = get_trigger_str((int)arg->trigger_uint8);

	snprintf(err_code_str, sizeof(err_code_str), "%04x", arg->error_code);

	ALOGI("%s: start:trigger - %s", __func__, trigger_str);
	snprintf(cmd, MAX_LOG_STR_KERNEL, "%s %s %s",
		 log_collection_script, trigger_str, err_code_str);
	gettimeofday(&t0, 0);

	status = wlbtd_popen(cmd);
	if (status == 0) {
		gettimeofday(&t1, 0);
		diff = (t1.tv_sec - t0.tv_sec) * 1000.0f +
		       (t1.tv_usec - t0.tv_usec) / 1000.0f;

		ALOGI("%s:   end:trigger - %s status %d", __func__, trigger_str,
		      WEXITSTATUS(status));
		ALOGI("tar done in %0dms", (int)diff);
		snprintf(cmd, MAX_LOG_STR_KERNEL, "%s tar generated in %dms",
			 trigger_str, (int)diff);
		parse_memdump_file_wlbtd();

		if (arg->trigger_uint8 == SCSC_LOG_FW_PANIC)
			status = SCSC_WLBTD_FW_PANIC_TAR_GENERATED;
		else
			status = SCSC_WLBTD_OTHER_TAR_GENERATED;

		/* check if we need to copy the latest panic in dram*/
		wlbtd_log_in_dram();

		send_response_to_kernel(EVENT_SABLE, cmd, status);
	} else {
		ALOGE("generating tar failed. status %d", WEXITSTATUS(status));
		snprintf(cmd, MAX_LOG_STR_KERNEL,
			 "generating tar failed. status %d",
			 WEXITSTATUS(status));
		send_response_to_kernel(EVENT_SABLE, cmd,
					(arg->trigger_uint8 ==
					 SCSC_LOG_FW_PANIC) ?
						SCSC_WLBTD_FW_PANIC_ERR_TAR :
						SCSC_WLBTD_OTHER_ERR_TAR);
	}

	sable_gen_complete();

	/* Free the script_args passed by creator */
	ALOGI("free script_args %p", arg);
	free(arg);

	ALOGI("Exiting tar thread function\n");
	pthread_exit(0);
}

int wlbtd_process_sable_req(sable_req *msg)
{
	uint16_t error_code = msg->error_code;
	int sable_fd, dev_fd, errn;
	int error = -1;
	int status = 0;
	char outfile_name[64];
	char errmsg[MAX_LOG_STR_KERNEL];
	const char *trigger_str;
	struct script_arguments *script_args = NULL;
	pthread_t tar_thread;
	pthread_attr_t attr;

	ALOGI("Processing sable request\n");
	if (!sable_gen_available()) {
		pthread_mutex_lock(&tar_cond_mutex);
		status = pthread_cond_wait(&tar_cond_var, &tar_cond_mutex);
		if (status != 0)
			ALOGE("pthread cond wait failed %d\n", status);
		pthread_mutex_unlock(&tar_cond_mutex);
	}
	sable_gen_in_progress();
	trigger_str = get_trigger_str((int)msg->trigger_uint8);
	ALOGI("Trigger_str  = %s\n", trigger_str);
	dev_fd = open(LOG_COLLECT_DEV_ENTRY, O_RDWR);
	if (dev_fd < 0) {
		errn = errno;
		ALOGE("failed %d to open %s\n", errn, LOG_COLLECT_DEV_ENTRY);
		snprintf(errmsg, MAX_LOG_STR_KERNEL,
			 "failed %d to open %s. No .sbl or tar generated.",
			 errn, LOG_COLLECT_DEV_ENTRY);
		send_response_to_kernel(EVENT_SABLE, errmsg,
					(msg->trigger_uint8 ==
					 SCSC_LOG_FW_PANIC) ?
						SCSC_WLBTD_FW_PANIC_ERR_NO_DEV :
						SCSC_WLBTD_OTHER_ERR_NO_DEV);
		sable_gen_complete();
		return -1;
	}

	char *addr = (char *)mmap(NULL, MAP_SIZE, PROT_READ | PROT_WRITE,
				  MAP_SHARED, dev_fd, 0);
	if (addr == nullptr) {
		errn = errno;
		ALOGE("mmap failed %d", errn);
		snprintf(errmsg, MAX_LOG_STR_KERNEL, "mmap failed %d", errn);
		send_response_to_kernel(EVENT_SABLE, errmsg,
					(msg->trigger_uint8 ==
					 SCSC_LOG_FW_PANIC) ?
						SCSC_WLBTD_FW_PANIC_ERR_MMAP :
						SCSC_WLBTD_OTHER_ERR_MMAP);
		close(dev_fd);
		sable_gen_complete();
		return -1;
	}

	snprintf(outfile_name, 64, "%s/.tmp-%s.sbl", SABLETARPATH, trigger_str);
	sable_fd = open(outfile_name, O_WRONLY | O_CREAT, 0644);
	if (sable_fd < 0) {
		errn = errno;
		ALOGE("failed %d to open %s\n", errn, outfile_name);
		snprintf(errmsg, MAX_LOG_STR_KERNEL, "failed %d to open %s",
			 errn, outfile_name);
		send_response_to_kernel(
			EVENT_SABLE, errmsg,
			(msg->trigger_uint8 == SCSC_LOG_FW_PANIC) ?
				SCSC_WLBTD_FW_PANIC_ERR_SABLE_FILE :
				SCSC_WLBTD_OTHER_ERR_SABLE_FILE);
		close(dev_fd);
		munmap((void *)addr, MAP_SIZE);
		sable_gen_complete();
		return -1;
	}

	/* write sable file */
	error = write(sable_fd, addr, MAP_SIZE);
	ALOGD("%s.sbl written", trigger_str);
	close(dev_fd);
	munmap((void *)addr, MAP_SIZE);
	close(sable_fd);
	/* wait until fw_panic moredump tar is ready before sending reply to driver */
	if (msg->trigger_uint8 != SCSC_LOG_FW_PANIC) {
		/* send response as soon as sable is generated and tar in the background */
		snprintf(errmsg, MAX_LOG_STR_KERNEL, "%s generated",
			 outfile_name);
		send_response_to_kernel(EVENT_SABLE, errmsg,
					SCSC_WLBTD_OTHER_SBL_GENERATED);
	}

	/* thread to create tar in parallel */
	script_args = (struct script_arguments *)malloc(
		sizeof(*script_args)); /* sable_tar_thread() must free */
	if (script_args == NULL) {
		snprintf(errmsg, MAX_LOG_STR_KERNEL, "no arg memory for %s",
			 trigger_str);
		send_response_to_kernel(
			EVENT_SABLE, errmsg,
			(msg->trigger_uint8 == SCSC_LOG_FW_PANIC) ?
				SCSC_WLBTD_FW_PANIC_ERR_SABLE_FILE :
				SCSC_WLBTD_OTHER_ERR_SABLE_FILE);
	} else {
		ALOGI("script_args %p (trigger %u, code %u)", script_args,
		      msg->trigger_uint8, error_code);
		script_args->trigger_uint8 = msg->trigger_uint8;
		script_args->error_code = error_code;
		/*
                 * Initialize and set thread attribute as detached as we won't wait for it with pthread_join()
                 * When detached thread terminates its resources are released immediately as opposed to wait for thread to be joined
                 * before resources are released
                 */
		pthread_attr_init(&attr);
		pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
		pthread_create(&tar_thread, &attr, sable_tar_thread,
			       (void *)script_args);
		pthread_attr_destroy(&attr);
	}
	return 0;
}

void *sable_consumer_thread(void *data)
{
	sable_req *req = NULL;
	data = (void *)0;

	ALOGI("In sable msg consumer thread\n");
	while (!wlbtd_sable_term_signal) {
		pthread_mutex_lock(&sq.cond_var_lock);
		if (sableq_is_empty()) {
			ALOGI("Waiting on message in queue \n");
			pthread_cond_wait(&sq.cond_var, &sq.cond_var_lock);
		}

		ALOGI("Out of pthread_cond_wait\n");
		req = sableq_dequeue();
		pthread_mutex_unlock(&sq.cond_var_lock);
		if (req == NULL) {
			ALOGE("sable_consumer_thread: No entry found in sableq. This should not happen \n");
		} else {
			ALOGI("error code = %d request type = %d\n",
			      req->error_code, req->type);
			ALOGI("trigger_uint8 = %d trigger_str = %s\n",
			      req->trigger_uint8, req->trigger_str);
			wlbtd_process_sable_req(req);
			free(req);
		}
	}

	ALOGI("Exiting sable consumer thread %d\n", wlbtd_sable_term_signal);
	pthread_exit(0);
}

/*
 * Initialise Sable Trigger implementation infrastructure
 */
void wlbtd_sable_init(void)
{
	int i;
	int ret;

	for (i = SABLE_FW_PANIC_REQ; i < SABLE_LAST_REQ; i++) {
		ALOGI("Setting slot %d to null in sableq \n", i);
		sq.pos[i] = NULL;
	}

	wlbtd_sable_term_signal = 0;
	ALOGI("Creating Sable thread \n");
	/*
         * This thread process sable request from sableq and acts as consumer.
         * Main thread acts as producer, parses netlink request and adds to sableq.
         */
	ret = pthread_create(&spid, NULL, &sable_consumer_thread, NULL);
	if (ret != 0)
		printf("wlbtd_sable_init: Thread creation failed with error %d\n",
		       ret);
}

/*
 * Clean up resources used by sable trigger implementation
 */
void wlbtd_sable_destroy(void)
{
	int i;

	/* Set termination signal for consumer thread */
	wlbtd_sable_term_signal = 1;
	/* Signal the queue in case it is waiting for an item in queue */
	pthread_cond_signal(&sq.cond_var);
	/* Wait for consumer thread to exit */
	pthread_join(spid, NULL);

	for (i = SABLE_FW_PANIC_REQ; i < SABLE_LAST_REQ; i++) {
		if (sq.pos[i] != NULL) {
			ALOGI("Freeing slot %d in sableq\n", i);
			free(sq.pos[i]);
		}
	}
}
