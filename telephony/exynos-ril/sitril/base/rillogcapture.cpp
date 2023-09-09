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
 * rillogcapture.cpp
 *
 *  Created on: 2018. 4. 24.
 *  Modified on: 2020. 5. 2.
 *  Author: kh620.kim
 *  Author: sungwoo48.choi
 */

#include "rillogcapture.h"
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <dirent.h>

#include "rillog.h"
#include <librilutils/systemproperty.h>
#include <string>
#include <sstream>
#include <base/mutex.h>
#include <unistd.h>

#define MIN_CHUNK_SIZE      (1*1024*1024)            // 1 MiB
#define MAX_CHUNK_SIZE      (10*1024*1024)           // 10 MiB
#define MAX_NUM_OF_FILE     50
#define MAX_TOTAL_LOG_SIZE  (MAX_CHUNK_SIZE * MAX_NUM_OF_FILE)
#define DIR_BASE_DUMP       "/data/vendor/radio"
#define DIR_PREV            "prev"
#define DIR_CURRENT         "cur"
#define FILE_NAME_PREFIX    "rild.log."
#define MAX_BUF_SIZE        1024

IMPLEMENT_MODULE_TAG(RilLogCapture, RilLogCapture)

bool RilLogCapture::m_RilLogIsInited;

#define PROP_LOG_BASE_DIR           "persist.vendor.ril.log.base_dir"
#define PROP_LOG_CHUNK_SIZE         "persist.vendor.ril.log.chunk_size"
#define PROP_LOG_NUM_OF_FILE        "persist.vendor.ril.log.num_file"
#define PROP_LOG_LAST_INDEX         "vendor.ril.log.last_index"

// RilLogCapture must not call CRilLog::Log
#undef RilLogE
#undef RilLogW
#undef RilLogI
#undef RilLogV

#define RilLogE(format, ...) OSAL_RADIO_LOGE(format, ##__VA_ARGS__)
#define RilLogW(format, ...) OSAL_RADIO_LOGW(format, ##__VA_ARGS__)
#define RilLogI(format, ...) OSAL_RADIO_LOGI(format, ##__VA_ARGS__)
#define RilLogV(format, ...) OSAL_RADIO_LOGV(format, ##__VA_ARGS__)
#define RilLog  RilLogI


RilLogCapture::RilLogCapture() {
    m_pRilLogCaptureThread = NULL;
    m_nRilLogPipeW = -1;
    m_nRilLogPipeR = -1;
    m_RilLogIsInited = false;
    m_pRilLogRMutex = NULL;
    mBaseDir = "";
    mTotalSize = 0;
    mChunkSize = 0;
    mNumOfFile = 0;
    mPos = -1;
    mLog = NULL;
    mFileSize = 0;
    mFirstBoot = true;
    mBaseDirAccessible = false;
}

RilLogCapture::~RilLogCapture() {
    if (m_nRilLogPipeW != -1) {
        RilLogW("[%s::%s] Stop error: m_nRilLogPipeW is -1", TAG, __FUNCTION__);
        close(m_nRilLogPipeW);
        m_nRilLogPipeW = -1;
    }

    if (m_nRilLogPipeR != -1) {
        RilLogW("[%s::%s] Stop error: m_nRilLogPipeR is -1", TAG, __FUNCTION__);
        close(m_nRilLogPipeR);
        m_nRilLogPipeR = -1;
    }

    if (m_pRilLogRMutex) {
        delete m_pRilLogRMutex;
        m_pRilLogRMutex = NULL;
    }

    m_RilLogIsInited = false;
}

int RilLogCapture::Start() {
    RilLogV("[%s::%s] Start Ril Log capture", TAG, __FUNCTION__);
    m_pRilLogCaptureThread = new Thread(this);
    if (m_pRilLogCaptureThread == NULL || m_pRilLogCaptureThread->Start() < 0) {
        return -1;
    }
    return 0;
}

RilLogCapture *RilLogCapture::MakeInstance() {
    RilLog("[%s::%s] ++", TAG, __FUNCTION__);

    if (m_RilLogIsInited) {
        RilLogE("[%s::%s] already initialized", TAG, __FUNCTION__);
        return NULL;
    }

    RilLogCapture *instance = NULL;
    instance = new RilLogCapture();
    if (instance != NULL) {
        if (instance->Init() < 0) {
            delete instance ;
            instance = NULL;
        }
    }

    RilLog("[%s::%s] --", TAG, __FUNCTION__);
    return instance;
}

int RilLogCapture::Init(void) {
    m_pRilLogRMutex = new CMutex();
    if(m_pRilLogRMutex == NULL) {
        RilLogE("[%s::%s] Fail to create RilLogRMutex instance", TAG, __FUNCTION__);
        return -1;
    }

    if (OpenMessagePipe() < 0) {
        RilLogE("[%s::%s] Fail to open message pipe", TAG, __FUNCTION__);
        return -1;
    }

    // load configs
    LoadConfigs();

    m_RilLogIsInited = true;
    return 0;
}

int RilLogCapture::OpenMessagePipe() {
    RilLog("[%s::%s]", TAG, __FUNCTION__);
    if (m_nRilLogPipeW != -1) {
        RilLogE("[%s::%s] Open command pipe again(m_nRilLogPipeW)", TAG, __FUNCTION__);
        close(m_nRilLogPipeW);
        m_nRilLogPipeW = -1;
    }

    if (m_nRilLogPipeR != -1) {
        RilLogE("[%s::%s] Open command pipe again(m_nRilLogPipeR)", TAG, __FUNCTION__);
        close(m_nRilLogPipeR);
        m_nRilLogPipeR = -1;
    }

    int fds[2];
    int n = pipe2(fds, O_CLOEXEC);
    if (n < 0) {
        RilLogE("[%s::%s] Command pipe create fail", TAG, __FUNCTION__);
        return -1;
    }

    m_nRilLogPipeR = fds[0];
    m_nRilLogPipeW = fds[1];
    return 0;
}

int RilLogCapture::notifyNewRilLog(const char* rilLogMsg) {
    int ret = 0;

    if (m_nRilLogPipeW != -1 && rilLogMsg != NULL) {
        m_pRilLogRMutex->lock();
        const size_t BUF_SIZE = 2048;
        char buff[BUF_SIZE];

        struct tm *capT;
        struct timeval val;
        gettimeofday(&val, NULL);
        capT = localtime(&val.tv_sec);

        snprintf(buff, sizeof(buff)-1, "%02d-%02d %02d:%02d:%02d.%06ld\t%d\t%d\t",
                capT->tm_mon+1, capT->tm_mday, capT->tm_hour, capT->tm_min, capT->tm_sec, val.tv_usec, getpid(), gettid());


        const char *data = rilLogMsg;
        int totalLen = strlen(data);
        while (totalLen > 0) {
            char ch = *(data + totalLen - 1);
            if (!(ch == '\n' || ch == '\r')) {
                break;
            }
            totalLen--;
        } // end while (trim)

        int s = 0;
        for (int i = 0; i < totalLen; i++) {
            char ch = *(data + i);
            if (ch == '\n' || (i == totalLen - 1)) {
                size_t len = i - s + 1;
                // timestamp
                if (write(m_nRilLogPipeW, buff, strlen(buff)) <= 0) {
                    ret = -1;
                    break;
                }

                // message
                if (write(m_nRilLogPipeW, data + s, len) <= 0) {
                    ret = -1;
                    break;
                }
                s = i + 1;
            }
        } // end for i ~

        // adding LF finally
        if (write(m_nRilLogPipeW, "\n", 1) <= 0) {
            ret = -1;
        }

        m_pRilLogRMutex->unlock();
    } else {
        ret = -1;
    }
    return ret;
}

int rmdirs(const char *path, int is_error_stop) {
    DIR *dir_ptr = NULL;
    struct dirent *file = NULL;
    struct stat buf;
    char filename[1024];

    if ((dir_ptr = opendir(path)) == NULL)
        return unlink(path);

    while ((file = readdir(dir_ptr)) != NULL) {
        RilLogV("find %s/%s", path, file->d_name);
        if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
            continue;
        }

        snprintf(filename, sizeof(filename)-1, "%s/%s", path, file->d_name);
        if (lstat(filename, &buf) == -1) {
            continue;
        }

        if (S_ISDIR(buf.st_mode)) {
            if(rmdirs(filename, is_error_stop) == -1 && is_error_stop) {
                closedir(dir_ptr);
                return -1;
            }
        } else if (S_ISREG(buf.st_mode) || S_ISLNK(buf.st_mode)) {
            if (unlink(filename) == -1 && is_error_stop) {
                closedir(dir_ptr);
                return -1;
            }
        }
    }
    closedir(dir_ptr);

    return rmdir(path);
}

void RilLogCapture::Run() {
    RilLogI("[%s] Start RilLogCapture %d,%d", TAG, mBaseDirAccessible, mFirstBoot);
    if (mBaseDirAccessible) {
        if (mFirstBoot) {
            std::string prevDir = GetPreviousLogDir();
            std::string curDir = GetCurrentLogDir();

            // remove prev logs and directory
            if (rmdirs(prevDir.c_str(), 1) != 0) {
                RilLogW("[%s] Failed to remove directory(%s)", TAG, strerror(errno));
            }

            // rename cur logs to prev
            if (rename(curDir.c_str(), prevDir.c_str()) != 0) {
                RilLogW("Failed to rename directory(%s)", strerror(errno));
            }

            if (MakeDirectory(curDir.c_str())) {
                mPos = 0;
            }
        } else {
            // Resume logging from the last index
            ResumeFromCrash();
        }
        RilLog("[%s] Starting with a log index %d", TAG, mPos);
    }

    bool isRunnable = true;
    if (m_nRilLogPipeR < 0 || !m_RilLogIsInited || mPos < -1) {
        RilLogV("[%s] Not initialized. Stop reader.", TAG);
        isRunnable = false;
    }

    char buf[MAX_BUF_SIZE];
    fd_set rfdSet;
    while(isRunnable) {
        if (mLog == NULL) {
            mLog = GetNextFileHandle(mPos);
            if (mLog == NULL) {
                // error
                // fail to create new log file
                RilLogW("[%s] Failed to open new log file.", TAG);
                break;
            }
        }

        int fd = m_nRilLogPipeR;
        FD_ZERO(&rfdSet);
        FD_SET(fd, &rfdSet);
        int n = select(fd + 1, &rfdSet, NULL, NULL, NULL);
        if (n > 0) {
            int r = read(fd, buf, sizeof(buf) - 1);
            if (r < 0) {
                if (errno == EINTR || errno == EAGAIN) {
                    continue;
                } else {
                    // error
                    RilLogW("[%s] Failed to read from pipe(%s)", TAG, strerror(errno));
                    break;
                }
            }

            buf[r] = 0;
            if (mLog != NULL) {
                fprintf(mLog, "%s", buf);
                fflush(mLog);
                mFileSize += r;

                if (mChunkSize > 0 && mFileSize >= mChunkSize) {
                    fclose(mLog);
                    mLog = NULL;
                    mFileSize = 0;
                    mPos = GetNextIndex();
                }
            }
        }
        else {
            RilLogW("[%s] unknown error (%s)", TAG, strerror(errno));
            break;
        }
    } // end while ~

    OnClose();
    RilLogI("[%s] Stop reader", TAG);
}

void RilLogCapture::OnClose() {
    // close the current log file
    if (mLog != NULL) {
        fclose(mLog);
        mLog = NULL;
        mFileSize = 0;
    }

    // close pipe
    if (m_nRilLogPipeR > 0) {
        close(m_nRilLogPipeR);
        m_nRilLogPipeR = -1;
    }

    if (m_nRilLogPipeW > 0) {
        close(m_nRilLogPipeW);
        m_nRilLogPipeW = -1;
    }
}

void RilLogCapture::LoadConfigs()
{
    // booting or silent reset
    int rilResetCount = SystemProperty::GetInt(PROPERTY_RIL_RESET, 0);
    RilLogV("[%s] ril.reset_count(%d)", TAG, rilResetCount);
    mFirstBoot = (rilResetCount == 0);

    // check base directory
    // base directory may not be created by a permission
    mBaseDir = GetValidBaseDirectory();
    mBaseDirAccessible = IsDirectoryExists(mBaseDir) || MakeDirectory(mBaseDir);

    // file configurations
    int chunkSize = SystemProperty::GetInt(PROP_LOG_CHUNK_SIZE, MIN_CHUNK_SIZE);
    if (chunkSize < MIN_CHUNK_SIZE) {
        chunkSize = MIN_CHUNK_SIZE;
    }
    else if (chunkSize > MAX_CHUNK_SIZE) {
        chunkSize = MAX_CHUNK_SIZE;
    }
    mChunkSize = chunkSize;

    int numOfFile = SystemProperty::GetInt(PROP_LOG_NUM_OF_FILE, MAX_NUM_OF_FILE);
    if (numOfFile <= 0 || numOfFile > MAX_NUM_OF_FILE) {
        numOfFile = MAX_NUM_OF_FILE;
    }
    mNumOfFile = numOfFile;

    if (mChunkSize > 0 && mNumOfFile > 0) {
        mTotalSize = mChunkSize * mNumOfFile;
    }

    RilLogV("[%s] mChunkSize=%zu mNumOfFile=%zu mTotalSize=%zu", TAG, mChunkSize, mNumOfFile, mTotalSize);
}

std::string RilLogCapture::GetValidBaseDirectory()
{
    std::string dir = SystemProperty::Get(PROP_LOG_BASE_DIR, DIR_BASE_DUMP);
    const char *startsWith = "/data/vendor";
    if (strncmp(startsWith, dir.c_str(), strlen(startsWith)) != 0) {
        RilLogW("[%s] Base log directory is not vendor_data_file. Use default.", TAG);
        dir = DIR_BASE_DUMP;
    }
    return dir;
}

bool RilLogCapture::IsDirectoryExists(std::string path)
{
    bool ret = access(path.c_str(), R_OK | W_OK) == 0;
    if (!ret) {
        RilLogW("[%s] Fail to access directory (%s)", TAG, strerror(errno));
    }
    return ret;
}

bool RilLogCapture::MakeDirectory(std::string path)
{
    mode_t mode = umask(02);
    bool ret = mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) == 0;
    if (!ret) {
        RilLogW("[%s] Failed to create directory (%s)", TAG, strerror(errno));
    }
    umask(mode);
    return ret;
}

std::string RilLogCapture::GetCurrentLogDir()
{
    std::stringstream ss;
    ss << mBaseDir << "/" << DIR_CURRENT;
    return ss.str();
}

std::string RilLogCapture::GetPreviousLogDir()
{
    std::stringstream ss;
    ss << mBaseDir << "/" << DIR_PREV;
    return ss.str();
}

int RilLogCapture::GetNextIndex()
{
    return (mPos + 1) % mNumOfFile;
}

int RilLogCapture::GetResumeIndex()
{
    size_t p = 0;
    p = SystemProperty::GetInt(PROP_LOG_LAST_INDEX, 0);
    if (p > mNumOfFile) {
        // reset to 0
        p = 0;
    }
    return p;
}

bool RilLogCapture::ResumeFromCrash()
{
    mPos = GetResumeIndex();
    mode_t mode = umask(022);
    mLog = fopen(GetNextFilename(mPos).c_str(), "ae");
    if (mLog != NULL) {
        int rilResetCount = SystemProperty::GetInt(PROPERTY_RIL_RESET, 0);
        fprintf(mLog, "--------- resume from rild crash(%d)\n", rilResetCount);
        fflush(mLog);
        mFileSize = ftell(mLog);
        RilLog("Resume logging index=%d fileSize=%zu", mPos, mFileSize);
    }
    umask(mode);
    return (mLog != NULL);
}

std::string RilLogCapture::GetNextFilename(int index)
{
    std::stringstream ss;
    ss << GetCurrentLogDir() << "/" << FILE_NAME_PREFIX << index;
    return ss.str();
}

FILE *RilLogCapture::GetNextFileHandle(int index)
{
    mode_t mode = umask(022);
    std::string path = GetNextFilename(mPos);
    RilLogV("[%s] new file index %d", TAG, index);
    FILE *fp = fopen(path.c_str(), "we");
    umask(mode);
    SystemProperty::Set(PROP_LOG_LAST_INDEX, index);
    return fp;
}
