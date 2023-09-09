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
 * sdmdocmanager.cpp
 *
 *  Created on: 2021. 9. 7.
 */
#include "sdmdocmanager.h"
#include <random>
#include "log.h"
#include "sdmdocenv.h"
#include "systemproperty.h"
#include "textutils.h"

static inline int INTrand() {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dis(0, INT_MAX);
    return dis(gen);
}

/**
 * SdmDocManager
 */
static pthread_rwlock_t sRwlock = PTHREAD_RWLOCK_INITIALIZER;

SdmDocManager::SdmDocManager()
    : mUid(0), mSeq(0), mType(SDM_TYPE1) {
    init();
}

uint32_t SdmDocManager::increaseUid() {
    pthread_rwlock_t *rwlockPtr = &sRwlock;
    int ret = pthread_rwlock_wrlock(rwlockPtr);
    uint32_t uid = mUid++;
    mSeq = 0;   // reset seq no. for new uid
    ret = pthread_rwlock_unlock(rwlockPtr);
    return uid;
}

SdmDoc *SdmDocManager::newDocument() {
    SdmDoc *sdmDoc = nullptr;
    std::string filepath = getNewFilename();
    if (mType == SDM_TYPE2) {
        pthread_rwlock_t *rwlockPtr = &sRwlock;
        int ret = pthread_rwlock_wrlock(rwlockPtr);
        uint32_t uid = mUid;
        uint32_t seq = mSeq++;
        ret = pthread_rwlock_unlock(rwlockPtr);

        filepath += SDM_TYPE2_EXT;
        sdmDoc = SdmDocV2::newInstance(uid, seq);
    } else {
        filepath += SDM_TYPE1_EXT;
        sdmDoc = SdmDocV1::newInstance();
    }

    if (sdmDoc) {
        if (!sdmDoc->create(filepath)) {
            delete sdmDoc;
            sdmDoc = nullptr;
        }
    }
    return sdmDoc;
}

void SdmDocManager::init() {
    mUid = INTrand();
    mType = SdmDocEnv::getSdmType();
}

std::string SdmDocManager::getNewFilename() {
    std::ostringstream oss;
    oss << SdmDocEnv::getBaseDir() <<
        SdmDocManager::getFilename(SdmDocEnv::getFilenamePrefix(), SdmDocEnv::getCurrentTime());
    return oss.str();
}

std::string SdmDocManager::getFilename(const std::string& prefix, uint64_t date) {
    auto yyyymmdd = static_cast<unsigned long>(date / 1000000);
    auto hhmmss = static_cast<unsigned long>(date % 1000000);
    std::string strFormat = !prefix.empty() ? "%s_%lu_%06lu" : "%s_%lu%06lu";
    std::string strPrefix = !prefix.empty() ? prefix : "sbuff";
    return string_format(strFormat, strPrefix.c_str(), yyyymmdd, hhmmss);
}