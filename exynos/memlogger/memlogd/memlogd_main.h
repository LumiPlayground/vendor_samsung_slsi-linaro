/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

#ifndef __MEMLOGD_MAIN_H__
#define __MEMLOGD_MAIN_H__

#ifdef LOG_TAG
#undef LOG_TAG
#endif

#define LOG_TAG "memlogd"

#define VDBG 1

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

//#include <private/android_filesystem_config.h>
#include <poll.h>
#include <sys/poll.h>
#include <utils/Log.h>

#include <dirent.h>
#include <sys/stat.h>
#include <stdlib.h>

#include <map>
#include <list>
#include <thread>
#include <string>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <regex>

#define PATH_CMD                "/sys/devices/platform/samsung,memlogger/cmd_info/cmd"
#define PATH_REQ                "/sys/devices/platform/samsung,memlogger/memlog-tree/"
#define PATH_DUMPSTATE          "/sys/devices/platform/samsung,memlogger/object_information/dumpstate"
#define SAVE_DIR                "/data/vendor/log/"
#define PATH_EVENT_DEV		"/dev/memlog-evt"
#define EVENT_CMD_PREFIX	"MLGCMD:"
#define BUILD_DEFAULT           "unknown"
#define MAX_FILE_NAME           128
#define MAX_BUF                 65536
#define MAX_MSG                 0xff
#define MAX_SIZE                0x100000 // 1024*1024
#define PADDING_SIZE            1024

#define TYPE_CMD                0
#define TYPE_LOG                1

#define ACK_RETRY_CNT           3
#define ACK_RETRY_WAIT          100000

#define BUILD_ENG               0
#define BUILD_USERDEBUG         1
#define BUILD_USER              2
#define BUILD_UNKNOWN           99

static const char *name_type[] = {
    "mask",
    "dump",
    "default",
    "array",
    "string",
    "direct",
    "file",
    "printf",
};

enum MEMLOG_CMD {
    CMD_CREATE = 1,
    CMD_PAUSE,
    CMD_RESUME,
    CMD_FREE,
    CMD_DUMP,
    CMD_SYNC,
};

struct fileInfo {
    uint64_t fp;
    uint32_t size;
};

#pragma pack(push, 1)
struct file_header {
    fileInfo header;
    char info[PADDING_SIZE - sizeof(fileInfo)];
};
#pragma pack(pop)

#pragma pack(push, 1)
struct memlog_file_cmd {
         uint32_t cmd;
         uint64_t max_file_size;
         uint32_t max_file_num;
         uint64_t polling_period;
         char file_name[128];
         char desc_name[128];
 };
#pragma pack(pop)

struct logPacket {
    uint16_t head; // HEAD_LOG
    uint8_t length;
    uint8_t level;
    char *msg;
};

class MemlogObject {
public:
    int inFd;               // file descriptor of device driver
    //int outFd;              // file descriptor of output file
    bool flag = false;
    bool enabled = true;
    bool paused = false;
    uint8_t level = 4;
    uint8_t maxFileCnt = 0;
    uint8_t curFileCnt = 0;
    uint64_t maxFileSize;
    uint64_t delay;
    uint64_t remain;
    FILE *pFile;
    std::string fileName;
    std::string descName;
    std::string treeName;
    std::map<int, std::string> fileNameList;
    int getHeader(fileInfo *header);
    void make_header(void);
    int cleanPath();
    FILE * get_Filep(void);
    std::string updateFileName(void);
    int64_t write(void);
    const char *getFileName(void);
    const char *getFileName(int i);
    void updateFileCnt(void);
};
void make_header(MemlogObject *obj);
void InputCmd(char input, MemlogObject *obj, const char *dst, unsigned int datalen);
void SetLevel(int level, void *data, unsigned int datalen);
void EnableFile(bool enabled, void *data, unsigned int datalen);
void scanCtrlList();
void saveDumpstate();
void free_file(MemlogObject *obj);
#endif
