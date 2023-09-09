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
 * DMFileManager.cpp
 *
 *  Created on: 2018. 5. 18.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <dirent.h>
#include <set>
#include "base/log.h"
#include "base/systemproperty.h"
#include "DMFileManager.h"
#include "sdmdocenv.h"

static DMFileManager *sInstance = NULL;
#define DISK_USED_THRESHHOLD 95
#define MANAGED_FILE_COUNT_THRESHHOLD 1000
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define MEGABYTE (1024 * 1024)

#define PROP_DIAG_LOG_CAPACITY              "persist.vendor.sys.diag.log.capacity"
#define PROP_DIAG_LOG_MANAGED_FILE_SIZE     "persist.vendor.sys.diag.log.managed_size"
#define PROP_DIAG_LOG_MAX_MANAGED_SIZE      "persist.vendor.sys.diag.log.maxsize"

static const char *IGNORE_FILE_LIST[] = {
    "NNEXT_PROFILE.nprf",
    "sbuff_profile.sdm",
    ".sbuff_header.sdm",
};

static const char *MANAGED_EXT[] = {
    ".sdm", ".gz", ".zip", "sdm2"
};

static bool isIgnoreFile(const std::string& fileName) {
    unsigned int size = (unsigned int)(sizeof(IGNORE_FILE_LIST) / sizeof(IGNORE_FILE_LIST[0]));

    for (unsigned int i = 0; i < size; i++) {
        std::string filter = IGNORE_FILE_LIST[i];
        if (fileName.compare(filter) == 0) {
            LOGD("matched in ignore file list. %s", filter.c_str());
            return true;
        }
    } // end for i ~
    return false;
}

static bool isManagedExt(const std::string& fileName) {
    unsigned int size = (unsigned int)(sizeof(MANAGED_EXT) / sizeof(MANAGED_EXT[0]));
    for (unsigned int i = 0; i < size; i++) {
        std::string ext = MANAGED_EXT[i];
        size_t pos = fileName.rfind(ext);
        if (pos != std::string::npos) {
            if (fileName.substr(pos).compare(ext) == 0) {
                LOGV("file ext is matched with %s", ext.c_str());
                return true;
            }
        }
    } // end for i ~
    return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * ZipWorkerListenerImpl
 */
struct ZipWorkerListenerImpl : public OnZipWorkerListener {
    DMFileManager *mFileManager;
    ZipWorkerListenerImpl(DMFileManager *fileManager) : mFileManager(fileManager) {}
    ~ZipWorkerListenerImpl()=default;

    void onZipStatusReport(ZipWorkerStatus status);
    void onZipArchiveComplete(const std::string& filepath, const std::string& archive, size_t remain);
    void onZipArchiveFailed(const std::string& filepath, size_t remain);
    void add(const std::string& filepath) {
        if (mFileManager) {
            mFileManager->add(filepath);
        }
    }
};

void ZipWorkerListenerImpl::onZipStatusReport(ZipWorkerStatus /* status */) {

}

void ZipWorkerListenerImpl::onZipArchiveComplete(const std::string& filepath,
        const std::string& archive, size_t /* remain */) {
    LOGD("onZipArchiveComplete: %s", filepath.c_str());

    // add a zip archive
    add(archive);

    // remove a source file
    if (remove(filepath.c_str()) != 0) {
        ALOGW("failed to remove %s", filepath.c_str());
    }
}

void ZipWorkerListenerImpl::onZipArchiveFailed(const std::string& filepath, size_t /* remain */) {
    LOGD("onZipArchiveFailed: %s", filepath.c_str());
    // add a source archive
    add(filepath);
}

///////////////////////////////////////////////////////////////////////////////////////////////////

DMFileManager::DMFileManager() : mCapacity(DISK_USED_THRESHHOLD), mManagedFileCount(0) {
    mTotalSize = 0;
    mMaxSize = 0;
    mZipWorkListener = nullptr;
}

DMFileManager::~DMFileManager() {
    if (mZipWorkListener) {
        delete mZipWorkListener;
    }
}

void DMFileManager::init() {
    refreshFileList();

    mZipWorkListener = new ZipWorkerListenerImpl(this);
    ZipHardWorker *zip = ZipHardWorker::getInstance();
    zip->addListener(mZipWorkListener);
}

void DMFileManager::createManagedFileList() {
    std::string baseDir = SdmDocEnv::getBaseDir();
    mTotalSize = 0;
    struct dirent **namelist;
    int n = scandir(baseDir.c_str(), &namelist, NULL, alphasort);
    if (n < 0) {
        LOGE("scandir error");
    } else {
        int i = 0;
        while (n--) {
            struct dirent *entry = namelist[i];
            if (entry->d_type == DT_REG) {
                std::string fileName = entry->d_name;
                if (isManagedExt(fileName) && !isIgnoreFile(fileName)) {
                    LOGV("[%zu] %s", mFileList.size(), fileName.c_str());
                    std::string filePath = baseDir + fileName;
                    struct DMFile dmFile;
                    dmFile.fileName = filePath;

                    std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
                    if (ifs.is_open() == 0) {
                        LOGE("cannot open file(%s)", filePath.c_str());
                    } else {
                        ifs.seekg(0, std::ios::end);
                        dmFile.fileSize = ifs.tellg();
                        dmFile.fileSize = (dmFile.fileSize + MEGABYTE) / MEGABYTE;  // MiB
                        // MiB
                        mTotalSize += dmFile.fileSize;
                        mFileList.push_back(dmFile);
                    }
                } else {
                    LOGV("not a managed file. (%s)", fileName.c_str());
                }
            }
            free(namelist[i++]);
        }  // end while ~
        free(namelist);
    }
    LOGD("Total managed file size: %u MiB, file counts: %zu", mTotalSize, mFileList.size());
}

void DMFileManager::shrink() {
    // MiB
    mMaxSize = SystemProperty::GetInt(PROP_DIAG_LOG_MAX_MANAGED_SIZE, 0);

    while (mFileList.size() > 1) {
        bool isRemovalRequiredByMaxSize = false;
        bool isRemovalRequiredByDiskUsages = false;
        std::string path = SdmDocEnv::getBaseDir();
        const std::string sdcard = "/sdcard";
        struct statvfs sb;
        if (path.compare(0, sdcard.size(), sdcard) != 0 && statvfs("/data", &sb) == 0) {
            uint32_t used = 100 - (sb.f_bavail * 100 / sb.f_blocks);
            if (VDBG || used > (uint32_t)(mCapacity * 0.85)) {
                LOGD("=====================================");
                LOGD("Use%%    : %u%%", used);
                LOGD("Capacity : %u%%", mCapacity);
                LOGV("f_bsize  : %10lu", sb.f_bsize);
                LOGV("f_blocks : %10lu", sb.f_blocks);
                LOGV("f_bavail : %10lu", sb.f_bavail);
                LOGD("=====================================");
            }
            if (used > mCapacity) {
                isRemovalRequiredByDiskUsages = true;
            }
        }

        if (VDBG || mTotalSize > (uint32_t)(mMaxSize * 0.85)) {
            LOGD("Total managed file size: %u MiB, Size limit: %u", mTotalSize, mMaxSize);
        }
        if (mMaxSize > 0 && mTotalSize > mMaxSize) {
            isRemovalRequiredByMaxSize = true;
        }

        if (isRemovalRequiredByDiskUsages || isRemovalRequiredByMaxSize) {
            struct DMFile dmFile = mFileList.front();
            mFileList.pop_front();
            std::string path = dmFile.fileName;
            if (unlink(path.c_str()) == 0) {
                int reason = (isRemovalRequiredByDiskUsages << 1) | isRemovalRequiredByMaxSize;
                LOGD("Remove an old file due to %d", reason);
                LOGV("Remove an old file %s", path.c_str());
            } else {
                LOGE("Remove an old file was failed. errno=%d", errno);
                LOGV("Failed to remove file %s. errno=%d", path.c_str(), errno);
            }
            mTotalSize -= dmFile.fileSize;
        } else {
            break;
        }
    } // end while ~

    while (mManagedFileCount > 0 && mFileList.size() > mManagedFileCount) {
        struct DMFile dmFile = mFileList.front();
        mFileList.pop_front();
        std::string path = dmFile.fileName;

        if (unlink(path.c_str()) == 0) {
            LOGD("Remove an old file due to exceeding max file count");
            LOGD("Remove an old file %s", path.c_str());
        } else {
            LOGE("Remove an old file was failed. errno=%d", errno);
            LOGV("Failed to remove file %s. errno=%d", path.c_str(), errno);
        }
        mTotalSize -= dmFile.fileSize;
    }

    LOGD("Remained managed file size: %zu", mFileList.size());
}

void DMFileManager::add(const std::string& fileName) {
    struct DMFile dmFile;
    dmFile.fileName = fileName;
    std::string filePath = fileName;
    std::ifstream ifs(filePath, std::ios::in | std::ios::binary);
    LOGD("ifs.is_open()=%d ifs.good()=%d ifs.bad()=%d", ifs.is_open(), ifs.good(), ifs.bad());
    if (ifs.is_open()) {
        ifs.seekg(0, std::ios::end);
        dmFile.fileSize = ifs.tellg();
        dmFile.fileSize = (dmFile.fileSize + MEGABYTE) / MEGABYTE;  // MiB
        mTotalSize += dmFile.fileSize;

        LOGD("DMFileManager::%s fileName=%s fileSize=%d MiB", __FUNCTION__, dmFile.fileName.c_str(), dmFile.fileSize);
        mFileList.push_back(dmFile);
        shrink();
    } else {
        LOGE("DMFileManager::%s cannot open file(%s)", __FUNCTION__, filePath.c_str());
    }
}

void DMFileManager::add(const SdmDoc *doc) {
    if (doc) {
        std::string filepath = doc->getFilepath();
        if (SdmDocEnv::isZipArachiveRequired(filepath)) {
            LOGD("ZipArachiveRequired for %s", doc->toString().c_str());
            ZipHardWorker *zip = ZipHardWorker::getInstance();
            zip->doZipArchive(filepath);
        } else {
            add(filepath);
        }
    }
}

void DMFileManager::removeAll() {
    LOGD("%s", __FUNCTION__);
    while (mFileList.size() > 0) {
        for (std::list<DMFile>::iterator iter = mFileList.begin();
                iter != mFileList.end();
                iter++) {
            struct DMFile dmFile = *iter;
            std::string path = SdmDocEnv::getBaseDir();
            path += dmFile.fileName;
            if (unlink(path.c_str()) < 0) {
                ALOGW("Failed to delete file %s.", dmFile.fileName.c_str());
                continue;
            }
            LOGD("Remove file %s", dmFile.fileName.c_str());
        } // end for ~
        mFileList.clear();
    }
    mTotalSize = 0;
}

void DMFileManager::refreshFileList() {
    LOGD("%s", __FUNCTION__);
    mTotalSize = 0;
    mFileList.clear();
    createManagedFileList();
    shrink();
}

void DMFileManager::setLimit(unsigned int capacity, unsigned int managedFileSize) {
    mCapacity = capacity;
    if (mCapacity == 0 || mCapacity > DISK_USED_THRESHHOLD)
        mCapacity = DISK_USED_THRESHHOLD;

    mManagedFileCount = managedFileSize;
    if (mManagedFileCount == 0 || mManagedFileCount > MANAGED_FILE_COUNT_THRESHHOLD)
        mManagedFileCount = MANAGED_FILE_COUNT_THRESHHOLD;

    LOGD("setLimit; capacity:%d managedFileSize:%d", mCapacity, mManagedFileCount);
}

DMFileManager *DMFileManager::getInstance() {
    if (sInstance == NULL) {
        sInstance = new DMFileManager();
        if (sInstance != NULL) {
            int capacity = -1;
            int managedFileSize = -1;
            capacity = SystemProperty::GetInt(PROP_DIAG_LOG_CAPACITY, DISK_USED_THRESHHOLD);
            managedFileSize = SystemProperty::GetInt(PROP_DIAG_LOG_MANAGED_FILE_SIZE,
                    MANAGED_FILE_COUNT_THRESHHOLD);
            sInstance->setLimit((unsigned int)capacity, (unsigned int)managedFileSize);
        }
    }
    return sInstance;
}

