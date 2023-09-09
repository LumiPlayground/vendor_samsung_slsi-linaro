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
 * silentlogging.h
 *
 *  Created on: 2021. 9. 14.
 */

#ifndef __SILENT_LOGGING_H__
#define __SILENT_LOGGING_H__

#include "diagmonitor.h"
#include "memblock.h"
#include "sdmdocdirector.h"
#include "ModemProxy.h"

/**
 * SilentLoggingBase
 */
struct SilentLoggingBase : public OnRawRequestListener {
    SdmDocDirector *mDocDirector;
    DMFileManager *mFileManager;
    ModemProxy *mModemProxy;
    bool mStarted = false;
    SilentLoggingBase(SdmDocDirector *docDirector, DMFileManager *fileManager);
    virtual ~SilentLoggingBase()=default;
    void setModemProxy(ModemProxy *modemProxy) { mModemProxy = modemProxy; }
    void refreshManageFileList();
    virtual void onModemDataReceive(const void *data, size_t datalen);
    void onRawRequest(const void *data, size_t datalen);
    virtual void onPrepare();
    virtual void onStart();
    virtual void onStop();
    virtual void onDestroy();
    virtual void onSendProfile(const void *data, size_t datalen);
    virtual void sendProfileIfNecessary() {}
};

/**
 * SilentLogging
 */
struct SilentLogging : public SilentLoggingBase {
    SilentLogging(SdmDocDirector *docDirector, DMFileManager *fileManager);
    void onPrepare();
    void onDestroy();
    void onSendProfile(const void *data, size_t datalen);
    void onCommand(int id, const void *data, size_t datalen);
    std::string getProfileFilepath();
    std::string getHeaderFilepath();
    void sendProfileIfNecessary();
    void saveProfile(const void *data, size_t datalen);
    void saveHeader(const void *data, size_t datalen);
    void removeProfile();
    void removeHeader();
    static bool isAutoStartRequired();
    static bool isAutoStartRequiredByConfig();
};

/**
 * SilentLoggingOnFactory
 */
struct SilentLoggingOnFactory : public SilentLoggingBase {
    MemBlock mProfile;
    SilentLoggingOnFactory(SdmDocDirector *docDirector, DMFileManager *fileManager);
    void onPrepare();
    void onDestroy();
    void onSendProfile(const void *data, size_t datalen);
    void sendProfileIfNecessary();
};

#endif /* EXTERNA__SILENT_LOGGING_H__L_DMD_SILENTLOGGING_H_ */
