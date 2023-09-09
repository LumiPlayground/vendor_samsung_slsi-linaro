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
 * silentlogging.cpp
 *
 *  Created on: 2021. 9. 15.
 */
#include "dmd_test_util.h"
#include "base/log.h"
#include "base/systemproperty.h"
#include "sdmdocenv.h"
#include "sdmstream.h"
#include "silentlogging.h"
#include "DMConstants.h"

#define MEBIBYTE                (1024 * 1024)
#define PROPERTY_SILENT_LOG_ON "vendor.sys.silentlog.on"
#define HEADER_FILE_NAME       ".sbuff_header.sdm"
#define PROFILE_NAME           "sbuff_profile.sdm"

/**
 * SilentLoggingBase
 */
SilentLoggingBase::SilentLoggingBase(SdmDocDirector *docDirector, DMFileManager *fileManager) :
    mDocDirector(docDirector), mFileManager(fileManager), mModemProxy(0) {

}

void SilentLoggingBase::onModemDataReceive(const void *data, size_t datalen) {
    if (mDocDirector) {
        mDocDirector->writeData(data, datalen);
    } else {
        LOGD("director is not valid. Please check the current mode.");
    }
}

void SilentLoggingBase::refreshManageFileList() {
    if (mFileManager) {
        mFileManager->refreshFileList();
    }
}

void SilentLoggingBase::onPrepare() {
    refreshManageFileList();
}

void SilentLoggingBase::onStart() {
    if (mDocDirector) {
        mDocDirector->onStart();
    }
    sendProfileIfNecessary();
    mStarted = true;
}

void SilentLoggingBase::onStop() {
    if (mStarted) {
        // stop logging
        if (mModemProxy) {
            mModemProxy->StopForcibly();
        }

        // stop writing sdm doc.
        if (mDocDirector) {
            mDocDirector->onStop();
        }
        mStarted = false;
    } else {
        LOGW("Not started yet");
    }
}

void SilentLoggingBase::onDestroy() {
}

void SilentLoggingBase::onSendProfile(const void *data, size_t datalen) {
    if (mModemProxy) {
        mModemProxy->WriteSafely(data, datalen);
    }
}

void SilentLoggingBase::onRawRequest(const void *data, size_t datalen) {
    if (mModemProxy) {
        mModemProxy->WriteSafely((void *)data, datalen);
    }
}

/**
 * SilentLogging
 */
SilentLogging::SilentLogging(SdmDocDirector *docDirector, DMFileManager *fileManager) :
        SilentLoggingBase(docDirector, fileManager) {
}

void SilentLogging::onSendProfile(const void *data, size_t datalen) {
    SilentLoggingBase::onSendProfile(data, datalen);

    // save profiles after sent to modem
    saveProfile(data, datalen);
}

void SilentLogging::onPrepare() {
    LOGV("SilentLogging::onPrepare");
    SilentLoggingBase::onPrepare();
    if (!mStarted) {
        SilentLoggingBase::onStart();
    }
    SystemProperty::Set(PROPERTY_SILENT_LOG_ON, "On");
}

void SilentLogging::onDestroy() {
    LOGV("SilentLogging::onDestroy");
    SilentLoggingBase::onDestroy();
    if (mStarted) {
        onStop();
    }
    removeProfile();
    removeHeader();
    SystemProperty::Set(PROPERTY_SILENT_LOG_ON, "");
}

void SilentLogging::onCommand(int id, const void * data, size_t datalen) {
    if (id == COMMAND_SET_USER_DEFINED_HEADER) {
        LOGV("SilentLogging::onCommand COMMAND_SET_USER_DEFINED_HEADER");
        saveHeader(data, datalen);
    }
}

std::string SilentLogging::getProfileFilepath() {
    return SdmDocEnv::getBaseDir() + PROFILE_NAME;
}

std::string SilentLogging::getHeaderFilepath() {
    return SdmDocEnv::getBaseDir() + HEADER_FILE_NAME;
}

void SilentLogging::saveProfile(const void *data, size_t datalen) {
    if (data && datalen > 0) {
        std::string filepath = getProfileFilepath();
        FILE *sprofile_file = fopen(filepath.c_str(), "abe");
        if(NULL != sprofile_file) {
            fwrite(data, 1, datalen, sprofile_file);
            fclose(sprofile_file);
            sprofile_file=NULL;
        } else {
            LOGD("----------Opening sprofile_file Fail-----\n");
        }
    }
}

void SilentLogging::saveHeader(const void *data, size_t datalen) {
    LOGD("saveHeader()");
    if (data && datalen > 0) {
        std::string filepath = getHeaderFilepath();
        FILE *sheader_file = fopen(filepath.c_str(), "abe");
        if(sheader_file != nullptr) {
            fwrite(data, 1, datalen, sheader_file);
            fclose(sheader_file);
            sheader_file = nullptr;
        } else {
            LOGD("----------Opening sheader_file Fail-----\n");
        }
    }
}

void SilentLogging::removeProfile() {
    std::string profilePath = getProfileFilepath();
    if (remove(profilePath.c_str()) != 0) {
        LOGD("----------Failed to remove profilePath-----");
    }

}

void SilentLogging::removeHeader() {
    std::string headerPath = getHeaderFilepath();
    if (remove(headerPath.c_str()) != 0) {
        LOGD("----------Failed to remove headerPath-----");
    }
}

void SilentLogging::sendProfileIfNecessary() {
    if (mModemProxy) {
        if (isAutoStartRequired()) {
            LOGD("[SilentLogging] Try auto start!");
            std::string filepath = SdmDocEnv::getBaseDir();
            filepath += "sbuff_profile.sdm";
            FileInputStream fis;
            if (fis.open(filepath)) {
                MemoryBufferOutputStream mos;
                int ret;
                char buf[2048] = {};
                while ((ret = fis.read(buf, sizeof(buf))) > 0) {
                    mos.write(buf, ret);
                }
                if (VDBG) {
                    LOGV("SilentLogging::sendProfileIfNecessary");
                    LOGV("====== profile hex dump ======");
                    LOGV("%s", ::toHexdump(mos.buf(), mos.size()).c_str());
                    LOGV("==============================");
                }
                // must call WriteSafely.
                // if not, modem can only receive the first DM packet(7F...7E)
                mModemProxy->WriteSafely(mos.buf(), mos.size());
                ret = true;
            } else {
                LOGE("[SilentLogging] Opening sprofile_file Fail.");
            }
        }
    } else {
        LOGD("[SilentLogging] No ModemProxy instance.");
    }
}

bool SilentLogging::isAutoStartRequired() {
    std::string filepath = SdmDocEnv::getBaseDir();
    filepath += "sbuff_profile.sdm";
    bool ret = false;
    if (access(filepath.c_str(), F_OK) != -1) {
        ret = true;
    }
    return ret;
}

bool SilentLogging::isAutoStartRequiredByConfig() {
    return (SystemProperty::GetInt("persist.vendor.config.dm.autostart", 1) == 1);
}

/**
 * SilentLoggingOnFactory
 */
SilentLoggingOnFactory::SilentLoggingOnFactory(SdmDocDirector *docDirector, DMFileManager *fileManager) :
        SilentLoggingBase(docDirector, fileManager) {
}

void SilentLoggingOnFactory::onPrepare() {
    LOGV("SilentLoggingOnFactory::onPrepare");
    SilentLoggingBase::onPrepare();
    if (!mStarted) {
        SilentLoggingBase::onStart();
    }
}

void SilentLoggingOnFactory::onDestroy() {
    LOGV("SilentLoggingOnFactory::onDestroy");
    SilentLoggingBase::onDestroy();
    if (mStarted) {
        SilentLoggingBase::onStop();
    }
    mProfile.clear();
}

void SilentLoggingOnFactory::onSendProfile(const void *data, size_t datalen) {
    LOGV("SilentLoggingOnFactory::onSendProfile");
    SilentLoggingBase::onSendProfile(data, datalen);
    mProfile.setData(data, datalen);
    if (VDBG) {
        LOGV("====== profile hex dump ======");
        LOGV("%s", ::toHexdump(mProfile.ptr(), mProfile.size()).c_str());
        LOGV("==============================");
    }
}

void SilentLoggingOnFactory::sendProfileIfNecessary() {
    if (mProfile.hasData()) {
        if (mModemProxy) {
            if (VDBG) {
                LOGV("SilentLoggingOnFactory::sendProfileIfNecessary");
                LOGV("====== profile hex dump ======");
                LOGV("%s", ::toHexdump(mProfile.ptr(), mProfile.size()).c_str());
                LOGV("==============================");
            }
            // must call WriteSafely.
            // if not, modem can only receive the first DM packet(7F...7E)
            mModemProxy->WriteSafely(mProfile.ptr(), mProfile.size());
        }
    } else {
        LOGD("SilentLoggingOnFactory: No profile is existed.");
    }
}
