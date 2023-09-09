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
 * zipworker_test.cpp
 *
 *  Created on: 2021. 9. 9.
 */
#include <gtest/gtest.h>
#include "dmd_test_util.h"
#include "memblock.h"
#include "zipworker.h"

#include <sstream>
#include <vector>

//TEST(ZipWorker, ZipOutputStream) {
//    srand(time(0));
//
//    std::string filename = "test.txt";
//    std::string filepath = "/data/local/tmp/dmd_tests/";
//    filepath += filename;
//    std::string zipName = filepath;
//    zipName += ".zip";
//
//    FileOutputStream fos(filepath);
//    TestVector testVector[10];
//    size_t size = sizeof(testVector) / sizeof(testVector[0]);
//    for (size_t i = 0; i < size; i++) {
//        std::string str = ::toHexdump(testVector[i].data, testVector[i].datalen);
//        fos.write(str.c_str(), str.length());
//    }
//    fos.close();
//
//    FileInputStream fis;
//    if (fis.open(filepath)) {
//        ZipOutputStream zos(zipName);
//        zos.putNextEntry("test_compress.txt");
//        zos.write(&fis);
//        zos.closeEntry();
//        zos.close();
//        fis.close();
//    }
//
//    std::stringstream ss;
//    ss << "unzip " << zipName << " > /dev/null 2>&1";
//    system(ss.str().c_str());
//
//    std::string unzipName = "test_compress.txt";
//    FileInputStream src(filepath), test(unzipName);
//    char buf[1024];
//    int r;
//    while ((r = src.read(buf, sizeof(buf))) > 0) {
//        MemBlock m1(buf, r);
//        r = test.read(buf, r);
//        MemBlock m2(buf, r);
//        EXPECT_TRUE(m1 == m2);
//    }
//    src.close();
//    test.close();
//    unlink(filepath.c_str());
//    unlink(zipName.c_str());
//    unlink(unzipName.c_str());
//}

TEST(ZipWorker, OnZipWorkerListener_onZipStatusReport) {
    struct Impl : public OnZipWorkerListener {
        bool active;
        bool idle;

        ~Impl() {
            active = false;
            idle = false;
        }
        void onZipStatusReport(ZipWorkerStatus status) {
            if (status == ZipWorkerStatus::ZIP_WORKER_IDLE) {
                idle = true;
            }

            if (status == ZipWorkerStatus::ZIP_WORKER_ACTIVE) {
                active = true;
            }
        }

        void onZipArchiveComplete(const std::string& /*filepath*/,
                const std::string& /*archive*/, size_t /*remain*/) {
        }

        void onZipArchiveFailed(const std::string& /*filepath*/, size_t /*remain*/) {
        }
    } lsn;
    ZipHardWorker *worker = ZipHardWorker::testInstance();
    EXPECT_NE(worker, nullptr);
    if (worker) {
        worker->addListener(&lsn);
        const char *filepath = "/data/local/tmp/dmd_tests/OnZipWorkerListener_onZipStatusReport";
        FileOutputStream fos(filepath);
        for (size_t i = 0; i < 10; i++) {
            TestVector t;
            fos.write(t.data, t.datalen);
        }
        fos.close();

        std::string zipfilepath = filepath;
        zipfilepath += ".zip";

        worker->doZipArchive(zipfilepath);
        worker->waitFor(0);
        worker->removeListener(&lsn);
        worker->interrupt();
        EXPECT_TRUE(lsn.idle);
        EXPECT_TRUE(lsn.active);

        remove(filepath);
        remove(zipfilepath.c_str());
    }
    ZipHardWorker::testRelease(worker);
    worker = nullptr;
}

TEST(ZipWorker, OnZipWorkerListener_onZipArchiveComplete) {
    struct Impl : public OnZipWorkerListener {
        size_t count;

        ~Impl() {
            count = 0;
        }
        void onZipStatusReport(ZipWorkerStatus /*status*/) {

        }

        void onZipArchiveComplete(const std::string& filepath,
                const std::string& archive, size_t /*remain*/) {
            count++;
            remove(filepath.c_str());
            remove(archive.c_str());
        }

        void onZipArchiveFailed(const std::string& /*filepath*/, size_t /*remain*/) {
        }
    } lsn;
    std::vector<std::string> testVector;
    ZipHardWorker *worker = ZipHardWorker::testInstance();
    EXPECT_NE(worker, nullptr);
    if (worker) {
        worker->addListener(&lsn);

        for (size_t i = 0; i < 100; i++) {
            std::string filepath = "/data/local/tmp/dmd_tests/OnZipWorkerListener_onZipArchiveComplete_";
            filepath += std::to_string(i);
            testVector.push_back(filepath);
            FileOutputStream fos(filepath);
            for (size_t j = 0; j < 50; j++) {
                TestVector t;
                fos.write(t.data, t.datalen);
            }
            fos.close();
        }

        for (auto it : testVector) {
            worker->doZipArchive(it);
            usleep(100);
        }
        worker->waitFor(0);
        worker->removeListener(&lsn);
        EXPECT_TRUE(lsn.count > 0 && lsn.count == testVector.size());

        for (auto it : testVector) {
            std::string filepath = it;
            std::string zipfilepath = filepath;
            zipfilepath += ".zip";
            remove(filepath.c_str());
            remove(zipfilepath.c_str());
        }

        worker->interrupt();
        ZipHardWorker::testRelease(worker);
        worker = nullptr;
    }
}

TEST(ZipWorker, OnZipWorkerListener_onZipArchiveFailed) {
    struct Impl : public OnZipWorkerListener {
        size_t count;

        ~Impl() {
            count = 0;
        }
        void onZipStatusReport(ZipWorkerStatus /*status*/) {

        }

        void onZipArchiveComplete(const std::string& /*filepath*/,
                const std::string& /*archive*/, size_t /*remain*/) {
        }

        void onZipArchiveFailed(const std::string& /*filepath*/, size_t /*remain*/) {
            count++;
        }
    } lsn;
    std::vector<std::string> testVector;
    ZipHardWorker *worker = ZipHardWorker::testInstance();
    EXPECT_NE(worker, nullptr);
    if (worker) {
        worker->addListener(&lsn);

        for (size_t i = 0; i < 100; i++) {
            std::string filepath = "/data/local/tmp/dmd_tests/OnZipWorkerListener_onZipArchiveFailed_";
            filepath += std::to_string(i);
            testVector.push_back(filepath);
        }

        for (auto it : testVector) {
            worker->doZipArchive(it);
            usleep(1000);
        }
        worker->waitFor(0);
        worker->removeListener(&lsn);
        EXPECT_TRUE(lsn.count > 0 && lsn.count == testVector.size());

        worker->interrupt();
        ZipHardWorker::testRelease(worker);
        worker = nullptr;
    }
}
