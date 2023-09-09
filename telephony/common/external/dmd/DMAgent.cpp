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
 * DMAgent.cpp
 *
 *  Created on: 2018. 5. 18.
 */
#include <string>
#include "log.h"
#include "zipworker.h"
#include "DMAgent.h"
#include "DMConstants.h"
#include "OemServiceManager.h"

#define SERVICE_NAME    "dm0"
#define SERVICE_NAME_FM "dm1"

static void OEM_OnRequest(int type, int id, void *data, unsigned int datalen);

static OemServiceManager sOemServiceManager;
static OEM_ServiceFunctions sOemServiceFunction = { OEM_OnRequest };


static int sActiveMode = MODE_NONE;
static void (*sActiveRespHandler)(const void *data, unsigned int datalen);

static void ExternalDM_OnProcess(const void *data, unsigned int datalen);
static void SilentLogging_OnProcess(const void *data, unsigned int datalen);
static void OnBoardApp_OnProcess(const void *data, unsigned int datalen);

/**
 * DMAgent
 */
struct DMAgentImpl : public DMAgent {
    DMAgentCallback *mCallback;
    AutologCallback *mAutologCallback;
    FileOperationCallback *mFileOperCallback;
    void onResponse(const void *data, size_t datalen) {
        DMAgent_OnResponse((void *)data, datalen);
    }

    void registerCallback(DMAgentCallback *callback) {
        mCallback = callback;
    }

    void onActiveModeChange(int oldMode, int newMode) {
        if (mCallback) {
            mCallback->onActiveModeChange(oldMode, newMode);
        }
    }

    void onSendProfile(const void *data, size_t datalen) {
        if (mCallback) {
            mCallback->onSendProfile(data, datalen);
        }
    }

    void onStart() {
        if (mCallback) {
            mCallback->onStart();
        }
    }

    void onStop() {
        if (mCallback) {
            mCallback->onStop();
        }
    }

    void onCommand(int id, const void *data, size_t datalen) {
        if (mCallback) {
            mCallback->onCommand(id, data, datalen);
        }
    }

    void onRawRequest(const void *data, size_t datalen) {
        if (mCallback) {
            mCallback->onRawRequest(data, datalen);
        }
    }

    void registerCallback(AutologCallback *callback) {
        mAutologCallback = callback;
    }

    int onSaveAutolog() {
        int ret = 0;
        if (mAutologCallback) {
            ret = mAutologCallback->onSaveAutolog();
        }
        return ret;
    }

    void registerCallback(FileOperationCallback *callback) {
        mFileOperCallback = callback;
    }

    void onRefreshManageFileList() {
        if (mFileOperCallback) {
            mFileOperCallback->onRefreshManageFileList();
        }
    }

    void onDocumentSizeChanged(size_t size) {
        if (mFileOperCallback) {
            mFileOperCallback->onDocumentSizeChanged(size);
        }
    }

    void onManagedFileCountChanged(size_t count) {
        if (mFileOperCallback) {
            mFileOperCallback->onManagedFileCountChanged(count);
        }
    }

    void onBaseDirectoryChanged(const std::string& dir) {
        if (mFileOperCallback) {
            mFileOperCallback->onBaseDirectoryChanged(dir);
        }
    }

    void onSetFilenamePrefix(const std::string& prefix) {
        if (mFileOperCallback) {
            mFileOperCallback->onSetFilenamePrefix(prefix);
        }
    }
};
static DMAgentImpl *sDMAgent = nullptr;

DMAgent *DMAgent_Init() {

    if (sDMAgent == nullptr) {
        // init once
        sDMAgent = new DMAgentImpl {};

        sActiveRespHandler = NULL;
        if (sOemServiceManager.init()) {
            sOemServiceManager.registerService(SERVICE_NAME, &sOemServiceFunction);
        }
    }
    return sDMAgent;
}

int DMAgent_OnResponse(const void *data, size_t datalen) {
    if (data != NULL && datalen > 0) {
        if (sActiveRespHandler != NULL) {
            sActiveRespHandler(data, datalen);
        }
        else {
            ALOGE("No active ResponseHandler. current mode is %d", sActiveMode);
        }
    }
    else {
        ALOGE("invalid response data");
    }

    return 0;
}

void ExternalDM_OnProcess(const void * /* data */, unsigned int /* datalen */) {

}

void SilentLogging_OnProcess(const void * /* data */, unsigned int /* datalen */) {

}

void OnBoardApp_OnProcess(const void *data, unsigned int datalen) {
    sOemServiceManager.notifyCallback(TYPE_RAW, COMMAND_NOTIFY_DM_LOG, (void *)data, datalen);
}

/**************************************************************************************/
/**************************************************************************************/
// types of handler
static void OnCommandHandler(int id, void *data, unsigned int datalen);
static void OnRawHandler(int id, void *data, unsigned int datalen);

// command handler
static void SetCurrentMode(int mode);
static void StopDM();
static void SaveAutoLog();

// Entry function from HIDL server
static void OEM_OnRequest(int type, int id, void *data, unsigned int datalen) {
    ALOGD("OEM_OnRequest type=%d id=%d, datalen=%u", type, id, datalen);

    if (type == TYPE_COMMAND) {
        OnCommandHandler(id, data, datalen);
    } else if (type == TYPE_RAW) {
        OnRawHandler(id, data, datalen);
    } else {
        ALOGE("unsupported type. type=%d", type);
    }
}

void OnCommandHandler(int id, void *data, unsigned int datalen) {
    // refactoring
    if (sDMAgent) {
        switch (id) {
        case COMMAND_SET_DM_MODE: {
            int newMode = *(int *)data;
            int oldMode = sActiveMode;
            if (oldMode != newMode) {
                sDMAgent->onActiveModeChange(oldMode, newMode);
                SetCurrentMode(newMode);
            }
            break;
        }
        case COMMAND_SEND_PROFILE:
            sDMAgent->onSendProfile(data, datalen);
            break;
        case COMMAND_STOP_DM:
            StopDM();
            break;
        case COMMAND_SAVE_AUTOLOG:
            SaveAutoLog();
            break;
        case COMMAND_REFRESH_FILE_LIST:
            sDMAgent->onRefreshManageFileList();
            break;
        case COMMAND_SET_DM_MAX_FILE_SIZE: {
            size_t size = *(uint32_t *)data * (1024 * 1024);
            sDMAgent->onDocumentSizeChanged(size);
            break;
        }
        case COMMAND_SET_DM_MAX_NUM_OF_FILE: {
            if (data && datalen >= sizeof(uint32_t)) {
                size_t count = *(uint32_t *)data;
                sDMAgent->onManagedFileCountChanged(count);
            }
            break;
        }
        case COMMAND_SET_DM_BASE_DIRECTORY: {
            if (data && datalen > sizeof(uint32_t)) {
                size_t len = *((uint32_t *)data);
                if (len > 0 && datalen >= sizeof(uint32_t) + len) {
                    const char *str = reinterpret_cast<const char *>((uint32_t *)data + 1);
                    sDMAgent->onBaseDirectoryChanged(std::string(str, len));
                }
            } else {
                LOGE("OnCommandHandler: COMMAND_SET_DM_BASE_DIRECTORY invalid parameters");
            }
            break;
        }
        case COMMAND_SET_DM_FILENAME_PREFIX: {
            std::string prefix = "";
            if (data && datalen > sizeof(uint32_t)) {
                size_t len = *((uint32_t *)data);
                if (len > 0 && datalen >= sizeof(uint32_t) + len) {
                    prefix = std::string(reinterpret_cast<const char *>((uint32_t *)data + 1), len);
                } else {
                    LOGE("OnCommandHandler: COMMAND_SET_DM_FILENAME_PREFIX invalid parameters");
                }
            }

            if (prefix.empty()) {
                LOGD("OnCommandHandler: COMMAND_SET_DM_FILENAME_PREFIX set to default");
            }
            sDMAgent->onSetFilenamePrefix(prefix);
            break;
        }
        default:
            sDMAgent->onCommand(id, data, datalen);
            break;
        }
    }
}

void OnRawHandler(int /* id */, void *data, unsigned int datalen) {
    if (sDMAgent) {
        sDMAgent->onRawRequest((const void *)data, datalen);
    }
}

void SetCurrentMode(int mode) {
    ALOGD("%s", __FUNCTION__);

    // init for further
    DMAgent_SetActiveMode(mode);
}

void DMAgent_SetActiveMode(int activeMode) {
    sActiveMode = activeMode;
    switch (sActiveMode) {
    case MODE_EXTERNAL_DM:
        sActiveRespHandler = ExternalDM_OnProcess;
        break;
    case MODE_SILENT_LOGGING:
        sActiveRespHandler = SilentLogging_OnProcess;
        break;
    case MODE_ON_BOARD_APP:
        sActiveRespHandler = OnBoardApp_OnProcess;
        break;
    case MODE_FACTORY_LOGGING:
        sActiveRespHandler = nullptr;
        break;
    default:
        ALOGE("Unsupported DMAgent. activeMode=%d", activeMode);
        sActiveMode = MODE_NONE;
        sActiveRespHandler = NULL;
        break;
    } // end switch ~
}

int DMAgent_GetActiveMode() {
    return sActiveMode;
}

void StopDM() {
    if (sDMAgent) {
        sDMAgent->onStop();
    }

    int ret = 1;
    // waiting for all ZIP work is finished.
    struct Impl : public OnZipWorkerListener {
        ~Impl()=default;
        void onZipStatusReport(ZipWorkerStatus /*status*/) {

        }

        void onZipArchiveComplete(const std::string& /*filepath*/,
                const std::string& /*archive*/, size_t remain) {
            LOGD("[DMAgent] %s remain=%zu", __FUNCTION__, remain);
            notifyRemainCount(remain);
        }

        void onZipArchiveFailed(const std::string& /*filepath*/, size_t remain) {
            LOGD("[DMAgent] %s remain=%zu", __FUNCTION__, remain);
            notifyRemainCount(remain);
        }

        void notifyRemainCount(size_t /*remain*/) {
            // TODO ???
        }
    } lsn;
    ZipHardWorker *worker = ZipHardWorker::getInstance();
    worker->addListener(&lsn);
    LOGD("[DMAgent] Waiting for stopping...");
    worker->waitFor(0);  // blocked till all zip operations are done.
    LOGD("[DMAgent] Completed.");
    worker->removeListener(&lsn);
    sOemServiceManager.notifyCallback(TYPE_COMMAND, COMMAND_STOP_DM, &ret, sizeof(int));
}

void SaveAutoLog() {
    LOGD("%s() Save Auto Log into files", __FUNCTION__);
    int ret = 0;
    if (sDMAgent) {
        ret = sDMAgent->onSaveAutolog();
    }
    sOemServiceManager.notifyCallback(TYPE_COMMAND, COMMAND_SAVE_AUTOLOG, &ret, sizeof(int));
}


/**************************************************************************************/
/**************************************************************************************/

struct FactoryDMAgentImpl : public FactoryDMAgent {
    FactoryDMCallback *mCallback;
    void registerCallback(FactoryDMCallback *callback) {
        mCallback = callback;
    }

    void onActiveModeChange(int oldMode, int newMode) {
        if (mCallback) {
            mCallback->onActiveModeChange(oldMode, newMode);
        }
    }

    void onStart() {
        if (mCallback) {
            mCallback->onStart();
        }
    }

    void onStop() {
        if (mCallback) {
            mCallback->onStop();
        }
    }

    void onSendProfile(const void *data, size_t datalen) {
        if (mCallback) {
            mCallback->onSendProfile(data, datalen);
        }
    }
};

FactoryDMAgentImpl *sFactoryDMAgent = nullptr;

static void FDM_OnRequest(int type, int id, void *data, unsigned int datalen);
static void OnFactoryCommandHandler(int id, void *data, unsigned int datalen);

static OemServiceManager sOemServiceManagerFm;
static OEM_ServiceFunctions sFactoryFunction = { FDM_OnRequest };

FactoryDMAgent *FactoryDMAgent_Init() {
    if (sFactoryDMAgent == nullptr) {
        // init once
        sFactoryDMAgent = new FactoryDMAgentImpl {};

        if (sOemServiceManagerFm.init()) {
            sOemServiceManagerFm.registerService(SERVICE_NAME_FM, &sFactoryFunction);
        }
    }
    return sFactoryDMAgent;
}

void OnFactoryCommandHandler(int id, void *data, unsigned int datalen) {
    ALOGD("%s", __FUNCTION__);
    if (id == FACTORY_COMMAND_SET_DM_MODE) {
        if (data == NULL || datalen < sizeof(int)) {
            ALOGE("Invalid parameters");
            return ;
        }

        int newMode = *((int *)data);
        int oldMode = sActiveMode;
        if (oldMode != newMode) {
            SetCurrentMode(newMode);
            if (sFactoryDMAgent) {
                sFactoryDMAgent->onActiveModeChange(oldMode, newMode);
                if (newMode == MODE_FACTORY_LOGGING) {
                    char *profile = NULL;
                    size_t profilelen = 0;
                    if (datalen >= sizeof(int)) {
                        profile = (char *)data + sizeof(int);
                        profilelen = datalen - sizeof(int);
                    }
                    sFactoryDMAgent->onSendProfile(profile, profilelen);
                }
            }
        }
    } else {
        ALOGE("unsupported id. id=%d", id);
    }
}

static void FDM_OnRequest(int type, int id, void *data, unsigned int datalen) {
    ALOGD("OEM_OnRequest type=%d id=%d, datalen=%u", type, id, datalen);
    if (type == TYPE_FACTORY_COMMAND) {
        OnFactoryCommandHandler(id, data, datalen);
    } else {
        ALOGE("unsupported type. type=%d", type);
    }
}
