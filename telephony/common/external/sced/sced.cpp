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
 * sced.cpp
 *
 * Created on: 2018. 05. 23
 */
#include "sced.h"
#include "OemServiceManager.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/wait.h>
#include <set>
#include <string>
#include <sys/types.h>
#include <signal.h>

#define SERVICE_NAME "sced0"
#define BUFFER_SIZE 512
#define MIN(a,b) ((a) < (b) ? (a) : (b))

static void OEM_OnRequest(int type, int id, void *data, unsigned int datalen);
static void DoTcpDump(int type, int id, char *data, unsigned int datalen);
static void DoKillChildProcess(const char* data);
static bool IsDigitsOnly(const char *str);

static OemServiceManager sOemServiceManager;
static OEM_ServiceFunctions sOemServiceFunction = { OEM_OnRequest };

std::set<pid_t> mPidList;

/**************************************************************************************/
// Entry function from HIDL server
static void OEM_OnRequest(int type, int id, void *data, unsigned int datalen) {
    char buf[BUFFER_SIZE +1] = {0, };
    if (data != nullptr) {
        strncpy(buf, (char *)data, MIN(datalen, BUFFER_SIZE));
    }

    ALOGD("%s, data: %s, size: %u", __FUNCTION__, buf, datalen);
    switch(id) {
        case COMMAND_TCP_DUMP:
        case COMMAND_TCP_DUMP_SNAPSHOT:
            DoTcpDump(type, id, buf, datalen);
            break;
        case COMMAND_KILL:
            DoKillChildProcess(buf);
            break;
        default:
            ALOGE("unsupported message id. id = %d", id);
            break;
    }
}
/**************************************************************************************/

void DoTcpDump(int type, int id, char *data, unsigned int datalen) {
    ALOGD("%s", __FUNCTION__);

    const char *cmdStartWith = "tcpdump_vendor -i";
    if (data == nullptr || strncasecmp(data, cmdStartWith, strlen(cmdStartWith)) != 0) {
        ALOGE("Invalid data.");
        return;
    }

    int pid = fork();
    if (pid == 0) {
        if (execl("/vendor/bin/sh", "/vendor/bin/sh", "-c", data, nullptr) < 0) {
            ALOGE("execl() fails. errno = %d", errno);
        }
    } else {
        mPidList.insert(pid);
        sOemServiceManager.notifyCallback(type, id, &pid, sizeof(int));
    }
}

void DoKillChildProcess(const char* data) {
    ALOGD("%s", __FUNCTION__);
    if (!IsDigitsOnly(data)) return;

    auto pid = static_cast<pid_t>(std::stoi(data));
    auto it = mPidList.find(pid);
    if (it != mPidList.end()) {
        if (kill(pid, SIGINT) < 0) {
            ALOGE("execl() fails. errno = %d", errno);
        } else {
            ALOGD("request success(%d)", pid);
            mPidList.erase(it);
       }
    } else {
        ALOGV("Invalid PID.");
    }
}

bool IsDigitsOnly(const char *str) {
    if (str == nullptr || *str == 0) {
        return false;
    }

    int len = strlen(str);
    for (int i = 0; i < len; i++) {
        char ch = *(str + i);
        if (ch < '0' || ch > '9') {
            return false;
        }
    } // end for i ~
    return true;
}

void destory(int sig)
{
    ALOGD("destory()");
    int status;
    pid_t pid;
    if (sig == SIGCHLD) {
        while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
            if (WIFEXITED(status)) { // WIFEXITED: check if child process is successfully terminated.
                ALOGD("PID(%d) is terminated", pid);
            }
        }
    }
}

int32_t main(void) {
    ALOGD("%s: Init OemServiceManager", __FUNCTION__);

    if (sOemServiceManager.init()) {
        sOemServiceManager.registerService(SERVICE_NAME, &sOemServiceFunction);
    }

    signal(SIGCHLD, destory);

    while(1) {
        sleep((unsigned int)-1);
    }
    return 0;
}
