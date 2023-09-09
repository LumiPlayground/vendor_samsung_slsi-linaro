/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <dirent.h>
#include <errno.h>
#include <sched.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <cutils/properties.h>
#include <pthread.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/capability.h>

#include <poll.h>
#include <sys/poll.h>
#include <utils/Log.h>

#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

#define MLG_OBJ_BASE_DIR "/sys/devices/platform/samsung,memlogger/memlog-tree/"

enum mlg_cmd {
	MLG_CMD_CAT,
	MLG_CMD_STRING,
	MLG_CMD_PAUSE,
	MLG_CMD_RESUME
};

char obj_dir[1024];
char data_node[1024];

int mlgcat_find_obj_dir(char *buf, size_t size,
			char *desc_name, char *obj_name)
{
	dirent **objlist;
	int list_num;
	int i;
	bool find_node = false;

	snprintf(buf, size, "%s%s", MLG_OBJ_BASE_DIR, desc_name);
	if (access(buf, F_OK)) {
		printf("%s: Fail to find obj DIR(%s)\n", __func__, buf);
		return -1;
	}

	list_num = scandir(buf, &objlist, NULL, alphasort);

	for (i = 0; i < list_num; i++) {
		char local_buf[1024];
		char read_buf[4096];
		int fd, ret;
		unsigned long cmp_count;

		if (objlist[i]->d_name[0] == '.')
			continue;
		snprintf(local_buf, sizeof(local_buf) - 1,
			"%s/%s/name", buf, objlist[i]->d_name);
		fd = open(local_buf, O_RDONLY);
		if (fd < 0) {
#ifdef DEBUG
			printf("%s: fail to open %s\n", __func__, local_buf);
#endif
			continue;
		}
		ret = read(fd, read_buf, sizeof(read_buf));
		close(fd);
		if (ret < 0) {
#ifdef DEBUG
			printf("%s: couldn't read file %s\n", __func__,
								local_buf);
#endif
			continue;
		}

		if (read_buf[strlen(read_buf) - 1] == '\n')
			read_buf[strlen(read_buf) - 1] = 0;
		cmp_count = strlen(obj_name) > strlen(read_buf) ?
					strlen(obj_name) : strlen(read_buf);

		if (!strncmp(read_buf, obj_name, cmp_count)) {
			find_node = true;
			snprintf(buf, size, "%s%s/%s", MLG_OBJ_BASE_DIR,
					desc_name, objlist[i]->d_name);
			break;
		}
	}

	for (i = 0; i < list_num; i++) {
		free(objlist[i]);
	}
	free(objlist);

	if (find_node == false) {
		printf("%s: there is no OBJ NODE(%s/%s)\n",
				__func__, desc_name, obj_name);
		return -1;
	}

	return 0;
}

int mlgcat_find_data_node(char *buf, size_t size,
			char *obj_info, char *obj_name)
{
	char read_buf[4096];
	char local_buf[1024];
	int fd, ret;
	char *file_info_ptr;
	int minor_num;

	snprintf(local_buf, sizeof(local_buf) - 1, "%s/info", obj_info);
	fd = open(local_buf, O_RDONLY);
	if (fd < 0) {
		printf("%s: fail to open %s\n", __func__, local_buf);
		return -1;
	}

	ret = read(fd, read_buf, sizeof(read_buf) - 1);
	close(fd);
	if (ret < 0) {
		printf("%s: fail to read %s\n", __func__, local_buf);
		return -1;
	}
	read_buf[ret] = 0;

	file_info_ptr = strstr(read_buf, "file_minor_num");
	if (!file_info_ptr) {
		printf("%s: fail to fild file_minor_num property\n", __func__);
		return -1;
	}

	sscanf(file_info_ptr, "file_minor_num=%d", &minor_num);
	snprintf(local_buf, sizeof(local_buf) - 1, "/dev/memlog-%d-%s",
							minor_num, obj_name);

	if (access(local_buf, F_OK)) {
		printf("%s: there is no data node (%s)\n",
					__func__, local_buf);
		return -1;
	}
	snprintf(buf, size, "%s", local_buf);

	return 0;
}

int mlgcat_parsing_emum_cmd(char *cmd)
{
	int ret;

	switch (*cmd) {
	case 'c':
		ret = MLG_CMD_CAT;
		break;
	case 'p':
		ret = MLG_CMD_PAUSE;
		break;
	case 'r':
		ret = MLG_CMD_RESUME;
		break;
	case 's':
		ret = MLG_CMD_STRING;
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

int mlgcat_sync_write(char *file_path, char *data)
{
	int fd, ret;

	if (access(file_path, F_OK)) {
		printf("%s: couldn't find sync node [%s]\n",
					__func__, file_path);
		return -1;
	}

	fd = open(file_path, O_WRONLY);
	if (fd < 0) {
		printf("%s: fail to open %s\n", __func__, file_path);
		return -1;
	}

	ret = write(fd, data, strlen(data));
	close(fd);
	if (ret <= 0) {
		printf("%s: fail to write %s\n", __func__, file_path);
		return -1;
	}

	return 0;
}

void mlgcat_pause(char *obj)
{
	char local_buf[1024];
	char cmd_buf[8];

	snprintf(local_buf, sizeof(local_buf) - 1, "%s/sync", obj);
	snprintf(cmd_buf, sizeof(cmd_buf), "p\n");
	mlgcat_sync_write(local_buf, cmd_buf);
}

void mlgcat_resume(char *obj)
{
	char local_buf[1024];
	char cmd_buf[8];

	snprintf(local_buf, sizeof(local_buf) - 1, "%s/sync", obj);
	snprintf(cmd_buf, sizeof(cmd_buf), "r\n");
	mlgcat_sync_write(local_buf, cmd_buf);
}

void mlgcat(char *obj, char *data, char *max_size)
{
	char cmd_file[1024];
	char cmd_buf[8];
	FILE *data_file;
	unsigned long long limit_size;
	unsigned long long total_read_cnt = 0;
	bool limit_size_en = false;
	bool is_string_mode = false;

	if (max_size) {
		sscanf(max_size, "%llx", &limit_size);
		if (!limit_size) {
			printf("%s: invalid max_size value argv[4]=[%s]\n",
							__func__, max_size);
			return;
		}
		limit_size_en = true;
	}

	snprintf(cmd_file, sizeof(cmd_file) - 1, "%s/sync", obj);
	snprintf(cmd_buf, sizeof(cmd_buf) - 1, "s\n");

	if (!strncmp(data, MLG_OBJ_BASE_DIR, strlen(MLG_OBJ_BASE_DIR)))
		is_string_mode = true;

	data_file = fopen(data, "rb");

	do {
		char read_buf[4096];
		size_t read_cnt;

		mlgcat_sync_write(cmd_file, cmd_buf);

		sleep(1);
		do {
			read_cnt = fread((void *)read_buf, 1,
					sizeof(read_buf) - 1, data_file);
			total_read_cnt += read_cnt;
			read_buf[read_cnt] = 0;
			printf("%s", read_buf);

			if (is_string_mode) {
				fclose(data_file);
				data_file = fopen(data, "rb");
			}

		} while (read_cnt != 0);

		if (limit_size_en) {
			if (total_read_cnt >= limit_size)
				break;
		}

	} while (true);

	fclose(data_file);
}

int main(int argc, char *argv[])
{
	int cmd;

	if (argc < 4) {
		printf("Invalid arguments\n");
		ALOGI("Invalid arguments");
		return 0;
	}

	if (mlgcat_find_obj_dir(obj_dir, sizeof(obj_dir) - 1,
						argv[1], argv[2])) {
		printf("MLGCAT: couldn't find obj dir\n");
		ALOGI("MLGCAT: couldn't find obj dir");
		return 0;
	}

	if (mlgcat_find_data_node(data_node, sizeof(data_node) - 1,
						obj_dir, argv[2])) {
		printf("MLGCAT: couldn't find data node\n");
		ALOGI("MLGCAT: couldn't find data node");
		return 0;
	}

	cmd = mlgcat_parsing_emum_cmd(argv[3]);
	switch (cmd) {
	case MLG_CMD_CAT:
		if (argc > 4)
			mlgcat(obj_dir, data_node, argv[4]);
		else
			mlgcat(obj_dir, data_node, NULL);
		break;
	case MLG_CMD_STRING:
	{
		FILE * d_file;

		d_file = fopen(data_node, "rb");
		snprintf(data_node, sizeof(data_node) - 1,
					"%s/to_string", obj_dir);
		if (argc > 4)
			mlgcat(obj_dir, data_node, argv[4]);
		else
			mlgcat(obj_dir, data_node, NULL);
		fclose(d_file);
	}
		break;
	case MLG_CMD_PAUSE:
		mlgcat_pause(obj_dir);
		break;
	case MLG_CMD_RESUME:
		mlgcat_resume(obj_dir);
		break;
	default:
		printf("MLGCAT: Invalid argv[3](cmd) %s\n", argv[3]);
		ALOGI("MLGCAT: Invalid argv[3](cmd) %s", argv[3]);
		break;
	}

	return 0;
}
