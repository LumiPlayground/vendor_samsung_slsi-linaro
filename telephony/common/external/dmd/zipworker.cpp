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
 * zipworker.cpp
 *
 *  Created on: 2021. 9. 9.
 */
#include <errno.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "log.h"
#include "sdmdocenv.h"
#include "zipworker.h"

#define KEYWORD_INTERRUPT "****interrupt****"

ZipHardWorker *ZipHardWorker::_instance = nullptr;

// thread unsafe. recommend first-call at the entry point of the process.
ZipHardWorker *ZipHardWorker::getInstance() {
    if (_instance == nullptr) {
        ZipHardWorker *inst = new ZipHardWorker();
        if (inst) {
            inst->init();
            _instance = inst;
        }
    }
    return _instance;
}

// thread unsafe. recommend release when the process ends.
void ZipHardWorker::releaseIntance() {
    if (_instance) {
        _instance->interrupt();
        delete _instance;
        _instance = nullptr;
    }
}

ZipHardWorker *ZipHardWorker::testInstance() {
    ZipHardWorker *worker = new ZipHardWorker;
    if (worker) {
        worker->init();
    }
    return worker;
}

void ZipHardWorker::testRelease(ZipHardWorker *worker) {
    if (worker) {
        delete worker;
    }
}

ZipHardWorker::ZipHardWorker() : mStatus(ZIP_WORKER_INIT), mWaitFor(false), mRunnable(false) {
    pthread_rwlock_init(&mListenerLock, nullptr);
    pthread_rwlock_init(&mWorkLock, nullptr);
    pthread_mutex_init(&mWorkerMutex, nullptr);
    pthread_cond_init(&mWorkerCond, nullptr);
    pthread_mutex_init(&mWaitForMutex, nullptr);
    pthread_cond_init(&mWaitForCond, nullptr);
}

ZipHardWorker::~ZipHardWorker() {
    mThread.Stop();

    pthread_rwlock_destroy(&mWorkLock);
    pthread_mutex_destroy(&mWorkerMutex);
    pthread_cond_destroy(&mWorkerCond);
    pthread_mutex_destroy(&mWaitForMutex);
    pthread_cond_destroy(&mWaitForCond);
}

ZipWorkerStatus ZipHardWorker::getStatus() {
    // lock might be necessary.
    return mStatus;
}

void ZipHardWorker::init() {
    mThread = Thread(this);
    mRunnable = true;
    if (mThread.Start() != 0) {
        mRunnable = false;
    }
}

size_t ZipHardWorker::getRemainCount() {
    pthread_rwlock_rdlock(&mWorkLock);
    size_t ret = mWork.size();
    pthread_rwlock_unlock(&mWorkLock);
    return ret;
}

bool ZipHardWorker::doZipArchive(const std::string& filepath) {
    bool ret = false;
    // filepath must not ends with "/".
    if (filepath.length() > 0 &&
        filepath.substr(filepath.length() - 1).compare("/") != 0) {
        pthread_rwlock_wrlock(&mWorkLock);
        mWork.push(filepath);
        pthread_rwlock_unlock(&mWorkLock);

        // wake up if suspended
        pthread_mutex_lock(&mWorkerMutex);
        pthread_cond_signal(&mWorkerCond);
        pthread_mutex_unlock(&mWorkerMutex);
        LOGD("doZipArchive request");
        ret = true;
    }
    return ret;
}

void ZipHardWorker::addListener(OnZipWorkerListener *listener) {
    if (listener) {
        pthread_rwlock_rdlock(&mListenerLock);
        bool found = false;
        for (auto lsn : mListener) {
            if (lsn == listener) {
                found = true;
                break;
            }
        }
        if (!found) {
            // unlock rdlock then hold wrlock
            pthread_rwlock_unlock(&mListenerLock);
            pthread_rwlock_wrlock(&mListenerLock);
            mListener.push_back(listener);
        }
        pthread_rwlock_unlock(&mListenerLock);
    }
}

void ZipHardWorker::removeListener(OnZipWorkerListener *listener) {
    if (listener) {
        pthread_rwlock_rdlock(&mListenerLock);
        for (auto it = std::begin(mListener); it != std::end(mListener); it++) {
            if (*it == listener) {
                // unlock rdlock then hold wrlock
                pthread_rwlock_unlock(&mListenerLock);
                pthread_rwlock_wrlock(&mListenerLock);
                mListener.erase(it);
                break;
            }
        }
        pthread_rwlock_unlock(&mListenerLock);
    }
}

bool ZipHardWorker::enterIdle() {
    LOGV("ZipHardWorker enterIdle");
    bool runnable = mRunnable;
    pthread_mutex_lock(&mWorkerMutex);
    while (mRunnable && getRemainCount() == 0) {
        notifyAll();

        ZipWorkerStatus oldStatus = mStatus;
        mStatus = ZIP_WORKER_IDLE;
        if (oldStatus != mStatus) {
            notifyZipStatusReport(mStatus);
        }

        pthread_cond_wait(&mWorkerCond, &mWorkerMutex);
    }
    pthread_mutex_unlock(&mWorkerMutex);
    return runnable;
}

void ZipHardWorker::notifyAll() {
    pthread_mutex_lock(&mWaitForMutex);
    if (mWaitFor) {
        mWaitFor = false;
        pthread_cond_broadcast(&mWaitForCond);
    }
    pthread_mutex_unlock(&mWaitForMutex);
}

void ZipHardWorker::waitFor(uint64_t /*millis*/) {
    pthread_mutex_lock(&mWaitForMutex);
    while (getRemainCount() > 0) {
        // increase mWaitFor count
        // pthread_cond_broadcast only if mWaitFor > 0.
        mWaitFor = true;
        pthread_cond_wait(&mWaitForCond, &mWaitForMutex);
        LOGD("%s wakeup", __FUNCTION__);
    }
    pthread_mutex_unlock(&mWaitForMutex);
}

void ZipHardWorker::notifyZipStatusReport(ZipWorkerStatus status) {
    pthread_rwlock_rdlock(&mListenerLock);
    for (auto lsn : mListener) {
        if (lsn) {
            lsn->onZipStatusReport(status);
        }
    }
    pthread_rwlock_unlock(&mListenerLock);
}

/**
 * @filepath a zip file path
 * @remain a count of remained source files to be compressed
 */
void ZipHardWorker::notifyZipArchiveComplete(const std::string& filepath,
        const std::string& archive, size_t remain) {
    pthread_rwlock_rdlock(&mListenerLock);
    for (auto lsn : mListener) {
        if (lsn) {
            lsn->onZipArchiveComplete(filepath, archive, remain);
        }
    }
    pthread_rwlock_unlock(&mListenerLock);
}

/**
 * @filepath a source file path
 * @remain a count of remained source files to be compressed
 */
void ZipHardWorker::notifyZipArchiveFailed(const std::string& filepath, size_t remain) {
    pthread_rwlock_rdlock(&mListenerLock);
    for (auto lsn : mListener) {
        if (lsn) {
            lsn->onZipArchiveFailed(filepath, remain);
        }
    }
    pthread_rwlock_unlock(&mListenerLock);
}

// ZipHardWork, just make a new ZIP archive.
void ZipHardWorker::Run() {
    LOGD("ZipHardWorker::Run");
    while (enterIdle()) {
        ZipWorkerStatus oldStatus = mStatus;
        mStatus = ZIP_WORKER_ACTIVE;
        LOGV("ZipHardWorker Active");
        if (oldStatus != mStatus) {
            notifyZipStatusReport(mStatus);
        }

        pthread_rwlock_rdlock(&mWorkLock);
        std::string filepath = mWork.front();
        pthread_rwlock_unlock(&mWorkLock);

        if (filepath.compare(KEYWORD_INTERRUPT) == 0) {
            continue;
        }

        LOGD("on ZIP archiving...");
        std::string filename = filepath;
        size_t pos = filepath.rfind("/");
        if (pos != std::string::npos) {
            // the next pos of the last "/".
            filename = filepath.substr(pos + 1);
        }

        // only support a single file compressing.
        // (TBD) support to make a zip archive of all files under the specified directory path.
        std::string archive = filepath;
        archive += ".zip";

        bool complete = false;
        FileInputStream fis;
        if (fis.open(filepath)) {
            ZipOutputStream zos(archive);
            if (zos.open(archive)) {
                zos.putNextEntry(filename);
                zos.write(&fis);
                zos.closeEntry();
                zos.close();
                fis.close();
                complete = true;
                sync(archive);
            }
        }
        LOGD("on ZIP archiving...done");
        pthread_rwlock_wrlock(&mWorkLock);
        mWork.pop();
        pthread_rwlock_unlock(&mWorkLock);

        if (complete && ::access(archive.c_str(), R_OK) == 0) {
            // broadcast onZipArchiveComplete
            notifyZipArchiveComplete(filepath, archive, getRemainCount());
        } else {
            // broadcast onZipArchiveFailed
            notifyZipArchiveFailed(filepath, getRemainCount());
        }
    }
    LOGD("ZipHardWorker::exit");
}

void ZipHardWorker::sync(const std::string& filepath) {
    int fd = open(filepath.c_str(), O_WRONLY | O_CLOEXEC);
    if (fd >= 0) {
        if (fsync(fd) < 0) {
            LOGD("sync on disk error. %d", errno);
        } else {
            LOGD("sync on disk.");
        }
        close(fd);
    }
}

void ZipHardWorker::interrupt() {
    mRunnable = false;
    doZipArchive(KEYWORD_INTERRUPT);
}

/**
 * ZipOutputStream
 */
ZipOutputStream::ZipOutputStream() : ZipOutputStream("") {

}

ZipOutputStream::ZipOutputStream(const std::string& filepath) : mFp(0), mZip(0), mIsEntry(false) {
    open(filepath);
}

ZipOutputStream::~ZipOutputStream() {
    close();
}

bool ZipOutputStream::open(const std::string& filepath) {
    if (mFp) {
        // already open
        return true;
    }

    bool ret = false;
    mFp = fopen(filepath.c_str(), "wbe");
    if (mFp) {
        mZip = new ZipWriter(mFp);
        ret = true;
    }
    return ret;
}

void ZipOutputStream::close() {
    if (mIsEntry) {
        if (mZip) {
            mZip->FinishEntry();
        }
        mIsEntry = false;
    }

    if (mZip) {
        mZip->Finish();
        delete mZip;
        mZip = nullptr;
    }

    if (mFp) {
        fclose(mFp);
        mFp = nullptr;
    }
}

int ZipOutputStream::write(const void *data, size_t datalen) {
    int ret = -1;
    if (data != nullptr && datalen > 0) {
        if (mZip && mIsEntry) {
            ret = mZip->WriteBytes(data, datalen);
        }
    }
    return ret;
}

int ZipOutputStream::write(SdmInputStream *inputStream) {
    int ret = -1;
    if (inputStream) {
        if (mZip && mIsEntry) {
            char buf[2048];
            int r;
            while ((r = inputStream->read(buf, sizeof(buf))) > 0) {
                ret = this->write(buf, r);
            }
        }
    }
    return ret;
}

void ZipOutputStream::putNextEntry(const std::string& entryName) {
    if (mZip && !mIsEntry) {
        mZip->StartEntry(entryName.c_str(), ZipWriter::kCompress | ZipWriter::kAlign32);
        mIsEntry = true;
    }
}

void ZipOutputStream::closeEntry() {
    if (mZip && mIsEntry) {
        mZip->FinishEntry();
        mIsEntry = false;
    }
}
