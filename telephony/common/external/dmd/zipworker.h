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
 * zipworker.h
 *
 *  Created on: 2021. 9. 9.
 */

#ifndef __ZIP_WORKER_H__
#define __ZIP_WORKER_H__

#include <pthread.h>
#include <queue>
#include <string>
#include <vector>
#include "sdmstream.h"
#include "thread.h"

// Android
#include <ziparchive/zip_writer.h>

enum ZipWorkerStatus {
    ZIP_WORKER_INIT = -1,
    ZIP_WORKER_IDLE = 0,
    ZIP_WORKER_ACTIVE = 1,
};

struct OnZipWorkerListener {
    virtual ~OnZipWorkerListener()=default;
    virtual void onZipStatusReport(ZipWorkerStatus status)=0;
    virtual void onZipArchiveComplete(const std::string& filepath,
            const std::string& archive, size_t remain)=0;
    virtual void onZipArchiveFailed(const std::string& filepath, size_t remain)=0;
};

class ZipHardWorker : public Runnable {
private:
    std::vector<OnZipWorkerListener *> mListener;
    pthread_rwlock_t mListenerLock;
    std::queue<std::string> mWork;
    ZipWorkerStatus mStatus;
    pthread_rwlock_t mWorkLock;

    pthread_mutex_t mWorkerMutex;
    pthread_cond_t mWorkerCond;
    pthread_mutex_t mWaitForMutex;
    pthread_cond_t mWaitForCond;
    bool mWaitFor;
    bool mRunnable;

    Thread mThread;

    ZipHardWorker();
    ~ZipHardWorker();

    void init();
    bool enterIdle();
    void notifyAll();
    void notifyZipStatusReport(ZipWorkerStatus status);
    void notifyZipArchiveComplete(const std::string& filepath, const std::string& archive,
            size_t remain);
    void notifyZipArchiveFailed(const std::string& filepath, size_t remain);
    void sync(const std::string& filepath);
    static ZipHardWorker *_instance;
public:
    ZipWorkerStatus getStatus();
    size_t getRemainCount();
    bool doZipArchive(const std::string& filepath);
    void addListener(OnZipWorkerListener *listener);
    void removeListener(OnZipWorkerListener *listener);
    void waitFor(uint64_t millis);
    void Run();
    void interrupt();

    static ZipHardWorker *getInstance();
    static void releaseIntance();

    // unit test only
    static ZipHardWorker *testInstance();
    static void testRelease(ZipHardWorker *worker);
};

/**
 * ZipOutputStream
 */
struct ZipOutputStream {
private:
    std::string mFilepath;
    FILE *mFp;
    ZipWriter *mZip;
    bool mIsEntry;
public:
    ZipOutputStream();
    ZipOutputStream(const ZipOutputStream &) = delete;
    ZipOutputStream& operator=(ZipOutputStream const&) = delete;
    ZipOutputStream(const std::string& filepath);
    ~ZipOutputStream();
    bool open(const std::string& filepath);
    void close();
    int write(const void *data, size_t datalen);
    int write(SdmInputStream *inputStream);
    void putNextEntry(const std::string& entryName);
    void closeEntry();
};

#endif /* EXTERNAL_DMD_ZIPWORKER_H_ */
