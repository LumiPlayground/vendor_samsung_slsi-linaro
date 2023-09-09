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
 * sdmdocenv.cpp
 *
 *  Created on: 2021. 9. 9.
 */
#include "sdmdoc.h"
#include "sdmdocenv.h"
#include "systemproperty.h"
#include "dmd_test_util.h"

#define DEFAULT_BASE_DIR   "/data/vendor/slog/"
#define PROPERTY_BASE_DIR  "vendor.sys.exynos.slog.path"
#define PROPERTY_FILENAME_PREFIX  "vendor.sys.exynos.slog.prefix"
#define MAX_FILENAME_PREFIX_LEN 32
#define SDM_DOC_MIN_SIZE     (2*1024*1024)     // 2 MiB
#define SDM_DOC_DEFAULT_SIZE (100*1024*1024)   // 100 MiB
#define SDM_DOC_MAX_SIZE     (200*1024*1024)   // 200 MiB

static size_t sSdmDocSize = SDM_DOC_DEFAULT_SIZE;

namespace SdmDocEnv {
std::string getBaseDir() {
    std::string baseDir =
            SystemProperty::Get(PROPERTY_BASE_DIR, DEFAULT_BASE_DIR);
    if (baseDir.length() == 0) {
        baseDir = DEFAULT_BASE_DIR;
    } else if (baseDir.back() != '/') {
        baseDir += "/";
    }
    return baseDir;
}

void setBaseDir(const std::string& dir) {
    SystemProperty::Set(PROPERTY_BASE_DIR, dir);
}

std::string getBaseDirDefault() {
    return DEFAULT_BASE_DIR;
}

std::string getFilenamePrefix() {
    return SystemProperty::Get(PROPERTY_FILENAME_PREFIX, "").substr(0, MAX_FILENAME_PREFIX_LEN);
}

void setFilenamePrefix(const std::string& prefix) {
    SystemProperty::Set(PROPERTY_FILENAME_PREFIX, prefix.substr(0, MAX_FILENAME_PREFIX_LEN));
}

int getSdmType() {
    int type = SDM_TYPE1;
    if (SystemProperty::GetInt("persist.vendor.config.sdm_type2", 0) == 1) {
        type = SDM_TYPE2;
    }
    return type;
}

std::string getSdmType1Ext() {
    return ".sdm";
}

std::string getSdmType2Ext() {
    return ".sdm2";
}

size_t getMaxDocSize() {
    return sSdmDocSize;
}

void setMaxDocSize(size_t size) {
    if (size < SDM_DOC_MIN_SIZE) {
        sSdmDocSize = SDM_DOC_MIN_SIZE;
    } else if (size > SDM_DOC_MAX_SIZE) {
        sSdmDocSize = SDM_DOC_MAX_SIZE;
    } else {
        sSdmDocSize = size;
    }
}

bool isZipArachiveRequiredByConfig() {
    return (SystemProperty::GetInt("persist.vendor.sys.dm.zip", 1) == 1);
}

// make a zip file if
//  1) - ".sdm" file
//  2) - allowed by a configuration (persist.vendor.sys.dm.zip)
bool isZipArachiveRequired(const std::string& filepath) {
    bool ret = false;
    if (isZipArachiveRequiredByConfig()) {
        std::string ext = SdmDocEnv::getSdmType1Ext();
        if (filepath.length() > ext.length() &&
            filepath.rfind(ext) != std::string::npos) {
            std::string str = filepath.substr(filepath.rfind(ext));
            ret = (str == ext);
        }
    }
    return ret;
}

uint64_t getCurrentTime() {
    uint64_t timestamp = 0UL;
    time_t timeSec;
    struct tm *pTmLocal;

    time(&timeSec);
    pTmLocal = localtime(&timeSec);

    timestamp = pTmLocal->tm_sec + (uint64_t)pTmLocal->tm_min*100 +
            (uint64_t)pTmLocal->tm_hour*10000UL +
            (uint64_t)pTmLocal->tm_mday*1000000UL +
            (uint64_t)(pTmLocal->tm_mon+1)*100000000UL +
            (uint64_t)(pTmLocal->tm_year+1900)*10000000000UL;
    return timestamp;
}
}
