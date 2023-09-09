/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */

/*
    RFSD Log
*/

#ifndef __RFS_LOG_H__
#define __RFS_LOG_H__

#define LOG_TAG "RFSD"
#define LOG_NDEBUG 1

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <utils/Log.h>
#include <unistd.h>
#include <time.h>

#define RFS_LOG_PATH        "/mnt/vendor/efs/err/"
#define MAX_FILE_NAME       128
#define DEFAULT_FILE_NAME   "rfslog"

class CRfsLog
{
    public:
        static void WriteRfsLog(const char* const func);
        template <typename... Targs>
        static void WriteRfsLog(const char* const func, const char* const param, Targs... args);
};

template <typename... Targs>
void CRfsLog::WriteRfsLog(const char* const func, const char* const param, Targs... args)
{
    char filestr[MAX_FILE_NAME];
    struct tm *t;
    time_t timer = time(NULL);

    if(chdir(RFS_LOG_PATH) < 0) {
        if(mkdir(RFS_LOG_PATH, S_IRWXU | S_IRWXG | S_IRWXO) < 0) {
            ALOGE("%s : mkdir %s create fail", __FUNCTION__, RFS_LOG_PATH);
            return;
        }
        else
            ALOGD("%s : %s", __FUNCTION__, RFS_LOG_PATH);
    }

    t = localtime(&timer);
    sprintf(filestr, "%s_%04d%02d%02d", DEFAULT_FILE_NAME, t->tm_year+1900, t->tm_mon+1, t->tm_mday);
    FILE *rfsLogFile = fopen(filestr, "a+e");

    if(rfsLogFile != NULL) {
        char buf[512] = {0, };
        //va_list ap;
        //va_start(ap, param);
        snprintf(buf, sizeof(buf) - 1, param, args...);
        //va_end(ap);

        fprintf(rfsLogFile, "%04d%02d%02d-%02d:%02d:%02d\t%-35s\t%s\n", t->tm_year+1900, t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, func, buf);
        fclose(rfsLogFile);
    }
}

#endif // __RFS_LOG_H__
