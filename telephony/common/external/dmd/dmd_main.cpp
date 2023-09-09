/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <sys/capability.h>
#include <sys/prctl.h>
#include <sys/stat.h>
#include <private/android_filesystem_config.h>

#include "base/log.h"
#include "DMAgent.h"
#include "DMFileManager.h"
#include "HostAgentManager.h"
#include "ModemProxy.h"

// DMD renewal
#include "dmd_test_util.h"
#include "dmagentcallbackimpl.h"
#include "dmstatemachine.h"
#include "dmstatefactoryimpl.h"
#include "diagmonitor.h"
#include "onboardlogging.h"
#include "silentlogging.h"
#include "sdmdocenv.h"
#include "sdmdocdirector.h"
#include "sdmdocmanager.h"
#include "systemproperty.h"
#include "timeutils.h"
#include "zipworker.h"
#include <sstream>

#define SNAPSHOT_DIR            "snapshot"
#define SANPSHOT_SLOG_FILENAME  "snapshot/silent_log.sdm"

#define TEST_ENV 0

///////////////////////////////////////////////////////////////////////////////

struct DiagMonitor {
    DMStateMachine mDsm;
    ModemProxy *mModemProxy;
    SilentLogging *mSilentLogging;
    SilentLoggingOnFactory *mFactoryLogging;
    OnBoardLogging *mOnBoardLogging;
    SdmDocManager *mSdmDocManager;
    SdmDocDirector *mSdmDocDirector;
    FileOperationCallbackImpl mFileOperationCallback;
    AutologImpl mAutolog;

    struct DMAgentCallbackImpl : DMAgentCallback {
        DiagMonitor& mDM;
        DMAgentCallbackImpl(DiagMonitor& dm) : mDM(dm) {
        }
        void onActiveModeChange(int oldMode, int newMode) {
            mDM.onActiveModeChange(oldMode, newMode);
        }
        void onStart() {
            SilentLogging *silentLogging = mDM.mSilentLogging;;
            if (silentLogging) {
                silentLogging->onStart();
            }
        }
        void onStop() {
            SilentLogging *silentLogging = mDM.mSilentLogging;;
            if (silentLogging) {
                silentLogging->onStop();
            }
        }
        void onSendProfile(const void *data, size_t datalen) {
            SilentLogging *silentLogging = mDM.mSilentLogging;;
            if (silentLogging) {
                silentLogging->onSendProfile(data, datalen);
            }
        }
        void onCommand(int id, const void *data, size_t datalen) {
            SilentLogging *silentLogging = mDM.mSilentLogging;;
            if (silentLogging) {
                silentLogging->onCommand(id, data, datalen);
            }
        }

        // RAW request can be sillent logging or OBDM.
        void onRawRequest(const void *data, size_t datalen) {
            OnRawRequestListener *onRawRequest = mDM.mOnRawRequest;
            if (onRawRequest) {
                onRawRequest->onRawRequest(data, datalen);
            }
        }
    };
    DMAgentCallbackImpl mDMCallback;

    struct FactoryDMCallbackImpl : public FactoryDMCallback {
        DiagMonitor& mDM;
        FactoryDMCallbackImpl(DiagMonitor& dm) : mDM(dm) {
        }
        void onActiveModeChange(int oldMode, int newMode) {
            if (newMode == MODE_SILENT_LOGGING) {
                mDM.mDsm.setState(DM_STATE_FACTORY_DM_STATE);
            } else {
                mDM.onActiveModeChange(oldMode, newMode);
            }
        }
        void onStart() {
            SilentLoggingOnFactory *factoryLogging = mDM.mFactoryLogging;
            if (factoryLogging) {
                factoryLogging->onStart();
            }
        }
        void onStop() {
            SilentLoggingOnFactory *factoryLogging = mDM.mFactoryLogging;
            if (factoryLogging) {
                factoryLogging->onStop();
            }
        }
        void onSendProfile(const void *data, size_t datalen) {
            SilentLoggingOnFactory *factoryLogging = mDM.mFactoryLogging;
            if (factoryLogging) {
                factoryLogging->onSendProfile(data, datalen);
            }
        }
    };
    FactoryDMCallbackImpl mFactoryCallback;

    OnRawRequestListener *mOnRawRequest;
    struct OnBoardResponseCallbackImp : public OnBoardResponseCallback {
        void onResponse(const void *data, size_t datalen) {
            DMAgent_OnResponse(data, datalen);
        }
    } mOnBoardResponseCallbackImpl;

    DiagMonitor();
    DiagMonitor(const DiagMonitor &) = delete;
    DiagMonitor& operator=(DiagMonitor const&) = delete;
    ~DiagMonitor();

    void start() {
        // ZipHardWorker - the first instantiate
        ZipHardWorker::getInstance();

        // Init DMFileManager
        DMFileManager *fileManager = DMFileManager::getInstance();
        if (fileManager != NULL) {
            fileManager->init();
        }
        mFileOperationCallback = FileOperationCallbackImpl(fileManager);

        // Init DM hosts (USB, socket, ...)
        HostAgentManager::Init();
        HostAgentManager *hostAgentManager = HostAgentManager::GetInstance();

        mModemProxy = ModemProxy::GetInstance();
        mModemProxy->Init();

        mSdmDocManager = new SdmDocManager();
        auto postActions = PostAction<std::function<void()>>();
        postActions.add([modemProxy = mModemProxy]() {
                ALOGD("PostAction: DM Common Metric Symbol");
                // common metric symbol
                static uint8_t packet[] = {
                    0x7F, 0x12, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
                    0xA0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x00,
                    0x00, 0x00, 0x00, 0x7E
                };
                modemProxy->Write(packet, sizeof(packet));
            });
        postActions.add([modemProxy = mModemProxy]() {
                ALOGD("PostAction: DM Internal Metric Symbol");
                // internal metric symbol
                static uint8_t packet[] = {
                    0x7F, 0x12, 0x00, 0x00, 0x0F, 0x00, 0x00, 0x00,
                    0xA0, 0x00, 0xF0, 0x00, 0x00, 0x00, 0x00, 0x25,
                    0x49, 0x4E, 0x40, 0x7E,
                };
                modemProxy->Write(packet, sizeof(packet));
            });
        mSdmDocDirector = new SdmDocDirector(mSdmDocManager, fileManager, postActions);
        mSilentLogging = new SilentLogging(mSdmDocDirector, fileManager);
        mFactoryLogging = new SilentLoggingOnFactory(mSdmDocDirector, fileManager);
        mOnBoardLogging = new OnBoardLogging(&mOnBoardResponseCallbackImpl);

        DMStateFactory *DMStatefactory =
                DMStateFactoryImpl::getInstance(mModemProxy, hostAgentManager, mSilentLogging,
                        mOnBoardLogging, mFactoryLogging);
        if (DMStatefactory) {
            mDsm.init(DMStatefactory);
            delete DMStatefactory;
            mModemProxy->SetModemDataReceiver(&mDsm);
            mModemProxy->SetModemStateLinstener(&mDsm);
        }

        // Init DMAgent
        DMAgent *DMAgent = DMAgent_Init();
        if (DMAgent) {
            DMAgent->registerCallback(&mDMCallback);
            DMAgent->registerCallback(&mFileOperationCallback);
            DMAgent->registerCallback(&mAutolog);
        }

        FactoryDMAgent *factoryDMAgent = FactoryDMAgent_Init();
        if (factoryDMAgent) {
            factoryDMAgent->registerCallback(&mFactoryCallback);
        }
    }

    void onActiveModeChange(int oldMode, int newMode) {
        switch (newMode) {
        case MODE_EXTERNAL_DM: {
            mDsm.setState(DM_STATE_HOST_DM_STATE);
            mOnRawRequest = nullptr;
            break;
        }
        case MODE_SILENT_LOGGING: {
            mDsm.setState(DM_STATE_SIELNT_LOGGING_STATE);
            mOnRawRequest = mSilentLogging;
            break;
        }
        case MODE_ON_BOARD_APP: {
            mDsm.setState(DM_STATE_ON_BOARD_APP_STATE);
            mOnRawRequest = mOnBoardLogging;
            break;
        }
        case MODE_FACTORY_LOGGING: {
            mDsm.setState(DM_STATE_FACTORY_DM_STATE);
            mOnRawRequest = nullptr;
            break;
        }
        default:
            LOGW("unsupported mode %d", newMode);
            LOGW("Active mode is changed from %d to %d", oldMode, newMode);
            mOnRawRequest = nullptr;
            break;
        }
    }
};

DiagMonitor::DiagMonitor() : mModemProxy(0), mSilentLogging(0),
        mFactoryLogging(0), mOnBoardLogging(0), mSdmDocManager(0), mSdmDocDirector(0),
        mDMCallback(*this), mFactoryCallback(*this), mOnRawRequest(0) {
}

DiagMonitor::~DiagMonitor() {
    if (mSdmDocDirector) {
        delete mSdmDocDirector;
    }

    if (mSdmDocManager) {
        delete mSdmDocManager;
    }

    if (mSilentLogging) {
        delete mSilentLogging;
    }

    if (mFactoryLogging) {
        delete mFactoryLogging;
    }
}

///////////////////////////////////////////////////////////////////////////////
int32_t main(void)
{
    const char *key = "vendor.sys.diag.start_count";
    int count = SystemProperty::GetInt(key, 0);
    LOGW("**************** Start DMD (%d) ****************", count);
    SystemProperty::Set(key, count + 1);

    //srand(time(0));
    umask(000);

#if TEST_ENV
    SdmDocEnv::setMaxDocSize(10000000);  // 10 MB
#endif // TEST_ENV

    DiagMonitor diagMonitor;
    diagMonitor.start();

    //forever
    while(true) {
        sleep(INT_MAX);
    }
    return 0;
}
