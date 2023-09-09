#ifndef __UTIL_SRINFO_H__
#define __UTIL_SRINFO_H__

#define SRINFO_PATH "/data/vendor/log/cbd"
#define SRINFO_LAST_PATH "/data/vendor/log/cbd/err"
#define SRINFO_FILE "sr_info"
#define SRINFO_LAST "last_sr_info"
#define SRINFO_MAX_SIZE 0x10000 /* 64KB */
#define SRINFO_READ_SIZE 0x1000 /* 4KB*/

struct shmem_srinfo {
	unsigned size;
	char buf[0];
};

enum srinfo_type {
	NO_SRINFO = 0,
	SRINFO_TYPE_1,
	SRINFO_TYPE_2,
	SRINFO_TYPE_3
};

void store_srinfo(char *surfix, int fd, enum srinfo_type reset_info_type);
void restore_srinfo(char *surfix, int fd, enum srinfo_type reset_info_type);

#endif
