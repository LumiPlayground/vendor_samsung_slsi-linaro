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
 * dmagentcallbackimpl.cpp
 *
 *  Created on: 2021. 10. 12.
 */
#include "base/log.h"
#include "base/systemproperty.h"
#include "dmagentcallbackimpl.h"
#include "sdmdocenv.h"
#include "timeutils.h"
#include "DMFileManager.h"
#include <errno.h>
#include <poll.h>
#include <sys/poll.h>
#include <string>

#define PATH_AUTOLOG            "/dev/ramdump_memshare"
#define MAX_PROP_LEN            128
#define KERNEL_TIMEOUT          50000
#define MAX_FILE_NAME           128
#define ERR2STR                 strerror(errno)
#define DEF_AUTOLOG_SIZE        32
#define MEGABYTE (1024 * 1024)
#define PROP_AUTOLOG_SIZE       "persist.vendor.sys.autolog.size"

int saveAutoLog();
int wait_event(int fd, short events, long timeout);

int saveAutoLog()
{
    int ret = 0;
    unsigned long mem_size, copied = 0;
    int dev_fd = -1;
    int autoLogFile = -1;
    char buff[PAGE_SIZE];

    // open device node
    dev_fd = open(PATH_AUTOLOG, O_RDWR | O_CLOEXEC);
    if (dev_fd < 0) {
        LOGE("[%s]%s open fail (%s)\n", __FUNCTION__, PATH_AUTOLOG, ERR2STR);
        return -ENODEV;
    }

    /* Get AutoLog size */
    mem_size = SystemProperty::GetInt(PROP_AUTOLOG_SIZE, DEF_AUTOLOG_SIZE) * MEGABYTE;

    /* open destination file */
    // To do.
    std::string srcFilePath = SdmDocEnv::getBaseDir();
    char srcFileName[MAX_FILE_NAME];
    sprintf(srcFileName, "autolog_%ld.btl", (long)timeutils::getCurrentTime());
    srcFilePath += srcFileName;

    autoLogFile = open(srcFilePath.c_str(), O_WRONLY | O_CREAT | O_CLOEXEC, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH);
    if (autoLogFile < 0) {
        LOGE("[%s]%s open fail (%s)\n", __FUNCTION__, srcFilePath.c_str(), ERR2STR);
        close(dev_fd);
        return -ENOENT;
    }

    /* Read & Save shared memory dump */
    while (copied < mem_size) {
        ret = wait_event(dev_fd, POLLIN, KERNEL_TIMEOUT);
        if (ret < 0) {
            LOGE("[%s]wait event fail!!\n", __FUNCTION__);
            goto exit;
        }

        /* Receive a DUMP frame from CP */
        ret = read(dev_fd, buff, sizeof(buff));
        if (ret < 0) {
            LOGE("[%s]DUMP read fail (%s)\n", __FUNCTION__, ERR2STR);
            goto exit;
        }

        /* not verified */
        copied += ret;

        ret = write(autoLogFile, buff, ret);
        if (ret < 0) {
            ALOGE("[%s]write fail (%s)\n", __FUNCTION__, ERR2STR);
            goto exit;
        }
    }

    if (fsync(autoLogFile))
        LOGE("[%s]fsync is failed (%s)\n", __FUNCTION__, ERR2STR);

    LOGD("[%s] save complete! (%lu bytes)\n", __FUNCTION__, copied);
exit:
    if (autoLogFile >= 0)
        close(autoLogFile);

    close(dev_fd);
    return ret;
}

int wait_event(int fd, short events, long timeout)
{
    int ret;
    struct pollfd pfd;

    pfd.fd = fd;
    pfd.events = events;
    while (1) {
        pfd.revents = 0;
        ret = poll(&pfd, 1, timeout);
        if (pfd.revents & events)
            break;

        if (pfd.revents == (POLLERR | POLLHUP)) {
            ret = -EIO;
            goto exit;
        } else if (pfd.revents & POLLHUP) {
            usleep(20000); //20ms wait
            continue;
        }
    }

    return 0;

exit:
    return ret;
}

/**
 * AutologImpl
 */
int AutologImpl::onSaveAutolog() {
    return saveAutoLog();
}

/**
 * FileOperationCallbackImpl
 */
void FileOperationCallbackImpl::onRefreshManageFileList() {
    if (mFileManager) {
        mFileManager->refreshFileList();
    }
}

void FileOperationCallbackImpl::onDocumentSizeChanged(size_t size) {
    SdmDocEnv::setMaxDocSize(size);
}

void FileOperationCallbackImpl::onManagedFileCountChanged(size_t count) {
    if (mFileManager) {
        mFileManager->setLimit(count);
    }
}

void FileOperationCallbackImpl::onBaseDirectoryChanged(const std::string& dir) {
    SdmDocEnv::setBaseDir(dir);
}

void FileOperationCallbackImpl::onSetFilenamePrefix(const std::string& prefix) {
    SdmDocEnv::setFilenamePrefix(prefix);
}