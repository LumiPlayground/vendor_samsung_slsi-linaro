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
 * rilappliaction.cpp
 *
 *  Created on: 2014. 11. 15.
 *      Author: sungwoo48.choi
 */

#include "rilapplication.h"
#include <libxml/parser.h>
#include <net/if.h>
#include "carrierloader.h"
#include "rilapplicationfactory.h"
#include "rilcontextwrapper.h"
#include "rilapptoken.h"
#include "modemstatemonitor.h"
#include "tokengen.h"
#include "operatortable.h"
#include "rilresponselistener.h"
#include "sapresponselistener.h"
/* RIL vendor external */
#include "rilexternalresponselistener.h"
#include "rillog.h"
#include "rilversioninfo.h"
#include "signal_handler.h"
#include "reset_util.h"
// Auto Verify PIN
#include "rildata.h"
#include "systemmessageid.h"
#include "telephonystatuslogging.h"

#include <librilutils/systemproperty.h>
#include <librilutils/textutils.h>
#include <librilutils/vendor.h>

#include <protocol/sit/factory/protocolfactorybuilder.h>
#include <unistd.h>

// add category to display selective logs
#undef RilLogI
#define RilLogI(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_INFO_LOG,  format, ##__VA_ARGS__)
#undef RilLogV
#define RilLogV(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_VERBOSE_LOG,  format, ##__VA_ARGS__)
#undef RilLogW
#define RilLogW(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_WARNING_LOG,  format, ##__VA_ARGS__)
#undef RilLogE
#define RilLogE(format, ...) CRilLog::Log(RIL_LOG_CAT_CORE, CRilLog::E_RIL_CRITICAL_LOG,  format, ##__VA_ARGS__)

IMPLEMENT_MODULE_TAG(RilApplication, RilApplication)

///////////////////////////////////////////////////////////////////////////////
// static
///////////////////////////////////////////////////////////////////////////////
RilApplication *RilApplication::instance = NULL;

RilApplication *RilApplication::GetInstance() {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    return instance;
}

void RilApplication::SetInstance(RilApplication *ptr) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    if(instance) delete instance;
    instance = ptr;
}

RilApplicationContext *RilApplication::GetRilApplicationContext() {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    return static_cast<RilApplicationContext *>(instance);
}

RilApplication * RilApplication::CreateInstance(const struct RIL_Env *pRilEnv, int utMode /*=0*/) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    if (pRilEnv != NULL) {
        if (instance == NULL) {
            instance = RilApplicationFactory::CreateRilApplication(pRilEnv);
            instance->SetUnitTestMode(utMode);

            // returns NULL if initialization is failed.
            if (instance->InitInstance() < 0) {
                delete instance;
                instance = NULL;

                // if RilApplication is NULL, RIL cannot work properly. try to reset RILD
                // to guarantee thread closing while initialization, wait some time before reset
                WaitForTimeUs(__FUNCTION__, 2'000'000U);
                RilReset("RilApplication Init Fail");
            }
        }
    }
    return instance;
}

void RilApplication::RIL_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id) {
    //RilLogV("%s::%s", TAG, __FUNCTION__);
    if (instance != NULL) {
        instance->OnRequest(request, data, datalen, t, socket_id);
    }
}
void RilApplication::RIL_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t) {
    //RilLogV("%s::%s", TAG, __FUNCTION__);
    if (instance != NULL) {
        instance->OnRequest(request, data, datalen, t);
    }
}
RIL_RadioState RilApplication::RIL_OnRadioStateRequest(RIL_SOCKET_ID socket_id) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    RIL_RadioState radioState = RADIO_STATE_UNAVAILABLE;
    if (instance != NULL) {
        radioState = instance->OnRadioStateRequest(socket_id);
    }
    return radioState;
}
RIL_RadioState RilApplication::RIL_OnRadioStateRequest() {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    RIL_RadioState radioState = RADIO_STATE_UNAVAILABLE;
    if (instance != NULL) {
        radioState = instance->OnRadioStateRequest();
    }
    return radioState;
}
int RilApplication::RIL_OnSupports(int /* requestCode */) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    // Not to support
    RilLogW("%s::%s Not support", TAG, __FUNCTION__);
    return 0;
}
void RilApplication::RIL_OnCancel(RIL_Token /* t */) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    // Not to support
    RilLogW("%s::%s Not support", TAG, __FUNCTION__);
}

const char *RilApplication::RIL_OnGetVersion() {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    // TODO returns Vendor RIL version string
    return RILVersionInfo::getString(true);
}

void RilApplication::RIL_OnTimedCallback(void * /* param */) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    // Not to support
    RilLogW("%s::%s Not support", TAG, __FUNCTION__);
}

// for unit test
void RilApplication::RIL_OnModemRspSend(void *data, unsigned int datalen) {
    if (instance != NULL && data != NULL && datalen > 0) {
        instance->OnModemRspSend(data, datalen);
    }
}

void RilApplication::RIL_OnSitrilStateUpdate(int radioState, int networkRegState, int cardState) {
    if (instance != NULL) {
        instance->OnSitrilStateUpdate(radioState, networkRegState, cardState);
    }
}

/* SAP */
void RilApplication::SAP_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    if (instance != NULL) {
        instance->OnSapRequest(request, data, datalen, t, socket_id);
    } else {
        RilLogW("No RilApplication instance. Vendor RIL may not be initialized.");
    }
}

void RilApplication::SAP_OnRequest(int request, void *data, unsigned int datalen, RIL_Token t) {
    RilLogI("%s::%s", TAG, __FUNCTION__);
    if (instance != NULL) {
        instance->OnSapRequest(request, data, datalen, t);
    } else {
        RilLogW("No RilApplication instance. Vendor RIL may not be initialized.");
    }
}

/* RIL vendor external */
void RilApplication::RIL_externalOnRequest(int reqOemId, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id) {
    if (instance != NULL) {
        instance->externalOnRequest(reqOemId, data, datalen, t, socket_id);
    } else {
        RilLogW("No RilApplication instance. Vendor RIL may not be initialized.");
    }
}

// Current supported RIL_VERSION
int RilApplication::RIL_VersionCode = RIL_VERSION;
int RilApplication::RIL_HalVersionCode = vendor::radio::getCurrentHalVersionCode();

///////////////////////////////////////////////////////////////////////////////
// OperatorDbMakeRunnable
///////////////////////////////////////////////////////////////////////////////
OperatorDbMakeRunnable::OperatorDbMakeRunnable() {
}

OperatorDbMakeRunnable::~OperatorDbMakeRunnable() {
}

void OperatorDbMakeRunnable::Run() {
    OperatorNameProvider::MakeInstance();
}

///////////////////////////////////////////////////////////////////////////////
// RilApplication
///////////////////////////////////////////////////////////////////////////////
RilApplication::RilApplication(const struct RIL_Env *pRilEnv/* = NULL*/)
    : m_pRilEnv(pRilEnv) {
    for (int i = 0; i < SIM_COUNT; i++) {
        m_RilContext[i] = NULL;
    } // end for i ~

    m_pModemStateMonitor = NULL;
    m_pRilRespListener = NULL;
    m_pSignalMonitor = NULL;

    m_pProductFactory = NULL;
    m_pServiceFactory = NULL;

    /* SAP */
    m_pSapEnv = NULL;
    m_pSapRespListener = NULL;

    /* RIL vendor external */
    m_pRilExternalEnv = NULL;
    m_pRilExternalRespListener = NULL;

    m_pOpDbThread = NULL;
    m_pOpDbRunnable = NULL;

    mUnitTestMode = 0;
    mUnitTestReqSerial = -1;
}

RilApplication::~RilApplication() {
    ExitInstance();
}

int RilApplication::InitInstance() {
    RilLogI("[%s] %s RIL_VERSION %d", TAG, __FUNCTION__, RilApplication::RIL_VersionCode);

    RilLogI("[%s] new  SignalMonitor", __FUNCTION__);
    m_pSignalMonitor = new SignalMonitor(this);
    if (m_pSignalMonitor != NULL) {
        RilLogI("[%s] start SignalMonitor", __FUNCTION__);
        m_pSignalMonitor->Start();
    }

    // Init TokenGen instance
    TokenGen::Init();

    /* RilApplicationProperty Set*/
    LoadConfigToRilProperty();

    // Dereived class MUST register RilContext instance
    if (OnInitialize() < 0) return -1;

    if (mUnitTestMode == 0) {    // skip in unitTestMode
        // init operator dB
        m_pOpDbRunnable = new OperatorDbMakeRunnable();
        m_pOpDbThread = new Thread(m_pOpDbRunnable);
        if ( m_pOpDbThread != NULL ) {
            if (m_pOpDbThread->Start() < 0) {
                RilLogE("Fail to start Operator DB creator");
            }
        } else {
            RilLogE("Can't create Operator DB Thread");
        }
    }

    m_pRilRespListener = new RilResponseListener(m_pRilEnv);

    if (mUnitTestMode == 0) {    // skip in unitTestMode
        m_pModemStateMonitor = ModemStateMonitor::MakeInstance(this);
        if (m_pModemStateMonitor == NULL || m_pModemStateMonitor->Start() < 0) {
            return -1;
        }

        TelephonyStatusLogging *logging = TelephonyStatusLogging::GetInstance();
        if (logging == NULL || logging->Start() == false) {
            RilLogE("[%s] %s Fail to start TelephonyStatusLogging", TAG, __FUNCTION__);
        }
    }

    xmlInitParser();

    ProtocolFactoryBuilder::createAllProtocolFactories();

    return 0;
}

// a carrier information of product
int RilApplication::LoadTargetOperator() {
    CarrierLoader &cl = CarrierLoader::GetInstance();
    int ret = cl.GetTargetOperator();
    if (ret < 0) {
        ret = cl.GetVendorTargetOperator();
    }
    return ret;
}

void RilApplication::LoadConfigToRilProperty() {
    /* build option : user / eng / ... */
    RilProperty *property = GetProperty();
    int nProp = -1;

    if ( property == NULL ) {
        RilLogW("cannnot get RIL application property");
        return;
    }

    std::string buildType = SystemProperty::Get(RO_BUILD_TYPE, "eng");
    if (buildType == std::string{"user"}) {
        RilLogI("[%s] user mode", __FUNCTION__);
        property->Put(RIL_APP_USERMODE, true);
    }

    std::string testSgc = SystemProperty::Get(RIL_SGC_TEST_CONFIG, "");
    if (TextUtils::IsDigitsOnly(testSgc)) {
        nProp = std::stoi(testSgc);
        RilLogI("[%s] target operator(sgc test): %d", __FUNCTION__, nProp);
        property->Put(RIL_APP_TARGET_OPER, nProp);
    }

    if (nProp < 0) {
        // check target operator
        nProp = LoadTargetOperator();
        RilLogI("[%s] target operator: %s(%d)", __FUNCTION__, buildType.c_str(), nProp);
        property->Put(RIL_APP_TARGET_OPER, nProp);
    }

    SystemProperty::Set("persist.vendor.radio.target_oper", nProp);

    // IWLAN operation mode
    string mode =SystemProperty::Get(RO_IWLAN_OPERATION_MODE, "");
    bool iwlanInLegacyMode = false;
    const char *IWLAN_OPERATION_MODE_LEGACY = "legacy";
    if (TextUtils::Equals(mode.c_str(), IWLAN_OPERATION_MODE_LEGACY) ||
            RilApplication::RIL_HalVersionCode < HAL_VERSION_CODE(1, 4)) {
        iwlanInLegacyMode = true;
    }
    m_RilAppProperty.Put(RIL_APP_IWLAN_IN_LEGACY_MODE, iwlanInLegacyMode);
    m_RilAppProperty.Put(RIL_APP_IWLAN_IN_AP_ASSIT_MODE, !iwlanInLegacyMode);
    RilLogV("[%s] %s=%s iwlanInLegacyMode=%d", __FUNCTION__, RO_IWLAN_OPERATION_MODE, mode.c_str(), iwlanInLegacyMode);
}

void RilApplication::ExitInstance() {
    RilLogI("[%s] %s", TAG, __FUNCTION__);

    // release resources of dereived class
    OnFinalize();

    if (m_pModemStateMonitor != NULL) {
        delete m_pModemStateMonitor;
        m_pModemStateMonitor = NULL;
    }

    for (int i = 0; i < SIM_COUNT; i++) {
        if (m_RilContext[i] != NULL) {
            delete m_RilContext[i];
            m_RilContext[i] = NULL;
        }
    } // end for i ~

    if (m_pRilRespListener != NULL) {
        delete m_pRilRespListener;
        m_pRilRespListener = NULL;
    }

    if (m_pSapRespListener) {
        delete m_pSapRespListener;
        m_pSapRespListener = nullptr;
    }

    if ( m_pSignalMonitor != NULL ) {
        m_pSignalMonitor->Stop();
        delete m_pSignalMonitor;
        m_pSignalMonitor = NULL;
    }

    /* RIL vendor external */
    if (m_pRilExternalRespListener != NULL) {
        delete m_pRilExternalRespListener;
        m_pRilExternalRespListener = NULL;
    }

    TelephonyStatusLogging::ResetInstance();
}

RilContext *RilApplication::GetDefaultRilContext() {
    return m_RilContext[RIL_SOCKET_1];
}

RilContext *RilApplication::GetRilContext(RIL_SOCKET_ID socket_id) {
    //RilLogI("[%s] %s socket_id=%d", TAG, __FUNCTION__, socket_id);
    if (socket_id < SIM_COUNT)
        return m_RilContext[socket_id];
    return NULL;
}

int RilApplication::RegisterRilContext(unsigned int id, RilContext *pRilContext) {
    if (id < SIM_COUNT) {
        if (m_RilContext[id] == NULL) {
            m_RilContext[id] = pRilContext;

            // init RilContext resources
            if (m_RilContext[id]->OnCreate() == 0) {
                return 0;
            }
        }
    }

    return -1;
}

int RilApplication::getIfIndex() {
    const std::string ifprefix = "rmnet";
    static int idx = 0;
    struct if_nameindex *tmp_list_addr, *idx_addr;

    // The if_nameindex() function shall return an array of if_nameindex structures, one structure per interface.
    // The end of the array is indicated by a structure with an if_index field of zero and an if_name field of NULL.
    tmp_list_addr = if_nameindex();

    if (tmp_list_addr != NULL) {
        for (idx_addr = tmp_list_addr; idx_addr->if_index != 0 || idx_addr->if_name != NULL; idx_addr++) {
            std::string ifname = idx_addr->if_name;
            if (ifname.find(ifprefix) != std::string::npos)
                idx++;
        }
    }

    if_freenameindex(tmp_list_addr);
    RilLogV("[%s] %s if index=%d", TAG, __FUNCTION__, idx/SIM_COUNT);
    return (idx/SIM_COUNT);
}

int RilApplication::OnInitialize()
{
    static unsigned int index = getIfIndex();

    // Create RilContext
    RilContextParam params[SIM_COUNT] = {
        {RIL_SOCKET_1, "/dev/umts_ipc0", "rmnet", 0, index, m_pProductFactory, m_pServiceFactory},
#ifdef ANDROID_MULTI_SIM
#if (SIM_COUNT >= 2)
        {RIL_SOCKET_2, "/dev/umts_ipc1", "rmnet", index, index, m_pProductFactory, m_pServiceFactory},
#endif
#endif // ANDROID_MULTI_SIM
    };

    for (int i = 0; i < SIM_COUNT; i++) {
        RilContext *pRilContext = new RilContextWrapper(this, &params[i]);
        if (pRilContext == NULL || RegisterRilContext(i, pRilContext) < 0) {
            return -1;
        }
    } // end for i ~

    // virtual sim feature
    SystemProperty::Set(RIL_UIM_REMOTE_MCC, "");
    SystemProperty::Set(RIL_UIM_REMOTE_SLOT, "-1");

#ifdef SUPPORT_NR
    int support = SystemProperty::GetInt(VENDOR_RIL_SUPPORT_NR, -1);
    if (support < 0) {
        SystemProperty::Set(VENDOR_RIL_SUPPORT_NR, "1");
    }
#endif

    // Setting false of this property makes that
    // TelephonyManager.doesSwitchMultiSimConfigTriggerReboot() returns true.
    // Device reboot is required until runtime multi-sim config switching is supported
    // by whole layers.
    SystemProperty::Set(REBOOT_REQUIRED_ON_MODEM_CHANGE, 0);

    // RilContext ready
    StartModemIpc();

    return 0;
}

int RilApplication::OnFinalize() {
    if (m_pOpDbThread != NULL) {
        m_pOpDbThread->Stop();

        if (m_pOpDbRunnable != NULL) {
            delete m_pOpDbRunnable;
            m_pOpDbRunnable = NULL;
        }

        delete m_pOpDbThread;
        m_pOpDbThread = NULL;
    }

    return 0;
}

void RilApplication::OnModemStateChanged(int state) {
    RilLogV("[%s] %s state=%d", TAG, __FUNCTION__, state);
    // TODO do something before invoke callback of RilContext

    for (int i = 0; i < SIM_COUNT; i++) {
        if (m_RilContext[i] != NULL) {
            m_RilContext[i]->OnModemStateChanged(state);
        }
    } // end for i ~
}

void RilApplication::StartModemIpc() {
    // invoke callback of RilContext
    for (int i = 0; i < SIM_COUNT; i++) {
        if (m_RilContext[i] != NULL) {
            if (m_RilContext[i]->OnStart() < 0) {
                ExitInstance();
                RilErrorReset("FAIL_TO_START_MODEMIPC1");
            }
        }
    } // end for i ~
}

void RilApplication::ResetModem(const char *reason) {
    if (m_pModemStateMonitor != NULL) {
        m_pModemStateMonitor->ResetModem(reason);
    }
}

#include "rilcontext.h"
RIL_SOCKET_ID RilApplication::GetProperSocketIdForOem(OEM_CATEGORY OemCategory) {
    RilLogV("[%s] %s Finding proper SOCKET ID for Category(%d)", TAG, __FUNCTION__, OemCategory);

    switch(OemCategory) {
    case OEM_CAT_AUDIO:
        {
            RilContext *pRilContext;
            RIL_SOCKET_ID socket_id_array[RIL_SOCKET_NUM] = {
                RIL_SOCKET_1,
#if (SIM_COUNT >= 2)
                RIL_SOCKET_2,
#if (SIM_COUNT >= 3)
                RIL_SOCKET_3,
#endif
#if (SIM_COUNT >= 4)
                RIL_SOCKET_4
#endif
#endif
            };
            RIL_SOCKET_ID socket_id;

            // If any rilcontext has an active call, return socket_id for that rilcontext <- can be applied only to DSDS
            ///TODO: need to consider DSDA condition
            for ( unsigned int i = 0; i < RIL_SOCKET_NUM; i++)
            {
                socket_id = socket_id_array[i];
                RilLogV("[%s] Find RilContext for socketid(%d)", __FUNCTION__, socket_id);
                pRilContext = GetRilContext(socket_id);
                if ( pRilContext == NULL ) break;

                RilProperty* ContextProperty = pRilContext->GetProperty();
                if ( ContextProperty == NULL ) break;

                if ( -1 != ContextProperty->GetInt(RIL_CONTEXT_CS_ACTIVE_CID, -1) )
                {
                    RilLogV("[%s] Found proper SOCKET ID for Category(%d) - Result SocketID : %d", __FUNCTION__, OemCategory, socket_id);
                    return socket_id;
                }

                RilLogW("[%s] No Active call in context of sock id(%d)", __FUNCTION__, socket_id);
            }
        }
        break;

    case OEM_CAT_IMS:
        {
            RilProperty *property = GetProperty();
            if (property != NULL) {
                int socketid = property->GetInt(RIL_APP_PS_ACTIVE_SIM, -1);
                if (socketid > -1) return (RIL_SOCKET_ID)socketid;
            }
            // return default RIL_SOCKET_ID
            return RIL_SOCKET_1;
        }
        break;
    case OEM_CAT_SIM:
        // return default RIL_SOCKET_ID
        return RIL_SOCKET_1;
    default:
        break;
    }

    RilLogW("[%s] Cannot find proper socketID - return default ID", __FUNCTION__);
    return RIL_SOCKET_1;
}

// @Overriding
void RilApplication::OnRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id) {
    //RilLogI("[%s] %s ", TAG, __FUNCTION__);
    UpdateUnitTestSerial(t);

    RilContext *pRilContext = GetRilContext(socket_id);
    if (pRilContext != NULL) {
        RilAppToken *token = new RilAppToken(m_pRilRespListener, t);
        pRilContext->OnRequest(request, data, datalen, (RIL_Token)token);
    }
}
void RilApplication::OnRequest(int request, void *data, unsigned int datalen, RIL_Token t) {
    OnRequest(request, data, datalen, t, RIL_SOCKET_1);
}

RIL_RadioState RilApplication::OnRadioStateRequest(RIL_SOCKET_ID socket_id) {
    //RilLogI("[%s] %s ", TAG, __FUNCTION__);
    RIL_RadioState radioState = RADIO_STATE_UNAVAILABLE;
    RilContext *pRilContext = GetRilContext(socket_id);
    if (pRilContext != NULL) {
        radioState = pRilContext->OnRadioStateRequest();
    }
    return radioState;
}

RIL_RadioState RilApplication::OnRadioStateRequest() {
    return OnRadioStateRequest(RIL_SOCKET_1);
}

void RilApplication::OnRequestComplete(RIL_Token t, RIL_Errno e, void *response, unsigned int responselen) {
    //RilLogI("[%s] %s", TAG, __FUNCTION__);
    if (t) {
        RilAppToken *token = static_cast<RilAppToken *>(t);
        if (token != NULL) {
            token->context->OnRequestComplete(token->t, e, response, responselen);
            delete token;
        }
    }
}
void RilApplication::OnUnsolicitedResponse(int unsolResponse, const void *data, unsigned int datalen) {
    //RilLogI("[%s] %s ", TAG, __FUNCTION__);
    // To the RIL daemon
    if (m_pRilRespListener != NULL) {
        m_pRilRespListener->OnUnsolicitedResponse(unsolResponse, data, datalen);
    }

    /* RIL vendor external */
    if (m_pRilExternalRespListener != NULL) {
        m_pRilExternalRespListener->OnUnsolicitedResponse(unsolResponse, data, datalen);
    }
}
void RilApplication::OnUnsolicitedResponse(int unsolResponse, const void *data, unsigned int datalen, RIL_SOCKET_ID socket_id) {
    //RilLogI("[%s] %s ", TAG, __FUNCTION__);

    // To the RIL daemon
    if (m_pRilRespListener != NULL) {
#ifdef ANDROID_MULTI_SIM
        m_pRilRespListener->OnUnsolicitedResponse(unsolResponse, data, datalen, socket_id);
#else
        m_pRilRespListener->OnUnsolicitedResponse(unsolResponse, data, datalen);
#endif // ANDROID_MULTI_SIM
    }

    /* RIL vendor external */
    if (m_pRilExternalRespListener != NULL) {
        m_pRilExternalRespListener->OnUnsolicitedResponse(unsolResponse, data, datalen, socket_id);
    }

    if (m_pSapRespListener != NULL) {
        if (unsolResponse > RIL_REQUEST_SAP_UNSOL_RESPONSESAP)
        {
            m_pSapRespListener->OnUnsolicitedResponse(unsolResponse, data, datalen, socket_id);
        }
    }
}

RIL_SOCKET_ID RilApplication::GetProperSocketId(int request, void * /* data */, unsigned int /* datalen */) {
    RIL_SOCKET_ID rilsockid = RIL_SOCKET_1;

    switch (request) {
    case RIL_REQUEST_SET_MUTE:
    case RIL_REQUEST_GET_MUTE:
    case RIL_REQUEST_OEM_VOLUME_SET :
    case RIL_REQUEST_OEM_VOLUME_GET :
    case RIL_REQUEST_OEM_AUDIO_PATH_SET:
    case RIL_REQUEST_OEM_AUDIO_PATH_GET:
    case RIL_REQUEST_OEM_MICROPHONE_SET:
    case RIL_REQUEST_OEM_MICROPHONE_GET:
    case RIL_REQUEST_OEM_AUDIO_CLOCK_SET:
    case RIL_REQUEST_OEM_AUDIO_LOOPBACK_SET:
        rilsockid = GetProperSocketIdForOem(OEM_CAT_AUDIO);
        break;
    case RIL_REQUEST_OEM_IMS_SET_CONFIGURATION :
    case RIL_REQUEST_OEM_IMS_GET_CONFIGURATION :
    case RIL_REQUEST_OEM_IMS_SIM_AUTH :
    case RIL_REQUEST_OEM_IMS_SET_EMERGENCY_CALL_STATUS :
    case RIL_REQUEST_OEM_IMS_SET_SRVCC_CALL_LIST :
    case RIL_REQUEST_OEM_IMS_GET_GBA_AUTH :
    case RIL_REQUEST_OEM_GET_IMS_SUPPORT_SERVICE:

    //AIMS
    case RIL_REQUEST_OEM_AIMS_DIAL:
    case RIL_REQUEST_OEM_AIMS_ANSWER:
    case RIL_REQUEST_OEM_AIMS_HANGUP:
    case RIL_REQUEST_OEM_AIMS_DEREGISTRATION:
    case RIL_REQUEST_OEM_AIMS_HIDDEN_MENU:
    case RIL_REQUEST_OEM_AIMS_ADD_PDN_INFO:
    case RIL_REQUEST_OEM_AIMS_CALL_MANAGE:
    case RIL_REQUEST_OEM_AIMS_SEND_DTMF:
    case RIL_REQUEST_OEM_AIMS_SET_FRAME_TIME:
    case RIL_REQUEST_OEM_AIMS_GET_FRAME_TIME:
    case RIL_REQUEST_OEM_AIMS_CALL_MODIFY:
    case RIL_REQUEST_OEM_AIMS_RESPONSE_CALL_MODIFY:
    case RIL_REQUEST_OEM_AIMS_TIME_INFO:
    case RIL_REQUEST_OEM_AIMS_CONF_CALL_ADD_REMOVE_USER:
    case RIL_REQUEST_OEM_AIMS_ENHANCED_CONF_CALL:
    case RIL_REQUEST_OEM_AIMS_GET_CALL_FORWARD_STATUS:
    case RIL_REQUEST_OEM_AIMS_SET_CALL_FORWARD_STATUS:
    case RIL_REQUEST_OEM_AIMS_GET_CALL_WAITING:
    case RIL_REQUEST_OEM_AIMS_SET_CALL_WAITING:
    case RIL_REQUEST_OEM_AIMS_GET_CALL_BARRING:
    case RIL_REQUEST_OEM_AIMS_SET_CALL_BARRING:
    case RIL_REQUEST_OEM_AIMS_SEND_SMS:
    case RIL_REQUEST_OEM_AIMS_SEND_EXPECT_MORE:
    case RIL_REQUEST_OEM_AIMS_SEND_SMS_ACK:
    case RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_SMS:
    case RIL_REQUEST_OEM_AIMS_CHG_BARRING_PWD:
    case RIL_REQUEST_OEM_AIMS_SEND_USSD_INFO:
    case RIL_REQUEST_OEM_AIMS_GET_PRESENTATION_SETTINGS:
    case RIL_REQUEST_OEM_AIMS_SET_PRESENTATION_SETTINGS:
    case RIL_REQUEST_OEM_AIMS_SET_SELF_CAPABILITY:
    case RIL_REQUEST_OEM_AIMS_HO_TO_WIFI_READY:
    case RIL_REQUEST_OEM_AIMS_HO_TO_WIFI_CANCEL_IND:
    case RIL_REQUEST_OEM_AIMS_HO_PAYLOAD_IND:
    case RIL_REQUEST_OEM_AIMS_HO_TO_3GPP:
    case RIL_REQUEST_OEM_AIMS_SEND_ACK_INCOMING_CDMA_SMS:
    case RIL_REQUEST_OEM_AIMS_MEDIA_STATE_IND:
    case RIL_REQUEST_OEM_AIMS_DEL_PDN_INFO:
    case RIL_REQUEST_OEM_AIMS_STACK_START_REQ:
    case RIL_REQUEST_OEM_AIMS_STACK_STOP_REQ:
    case RIL_REQUEST_OEM_AIMS_XCAPM_START_REQ:
    case RIL_REQUEST_OEM_AIMS_XCAPM_STOP_REQ:
    case RIL_REQUEST_OEM_AIMS_SET_GEO_LOCATION_INFO:
    case RIL_REQUEST_OEM_AIMS_CDMA_SEND_SMS:
    case RIL_REQUEST_OEM_AIMS_SET_PDN_EST_STATUS:
    case RIL_REQUEST_OEM_AIMS_SET_HIDDEN_MENU_ITEM:
    case RIL_REQUEST_OEM_AIMS_GET_HIDDEN_MENU_ITEM:
    case RIL_REQUEST_OEM_AIMS_SET_RTP_RX_STATISTICS:
    case RIL_REQUEST_OEM_WFC_SET_VOWIFI_HO_THRESHOLD:
    case RIL_REQUEST_OEM_AIMS_RTT_SEND_TEXT:
    case RIL_REQUEST_OEM_AIMS_EXIT_EMERGENCY_CB_MODE:
    case RIL_REQUEST_OEM_AIMS_RCS_MULTI_FRAME:
    case RIL_REQUEST_OEM_AIMS_RCS_CHAT:
    case RIL_REQUEST_OEM_AIMS_RCS_GROUP_CHAT:
    case RIL_REQUEST_OEM_AIMS_RCS_OFFLINE_MODE:
    case RIL_REQUEST_OEM_AIMS_RCS_FILE_TRANSFER:
    case RIL_REQUEST_OEM_AIMS_RCS_COMMON_MESSAGE:
    case RIL_REQUEST_OEM_AIMS_RCS_CONTENT_SHARE:
    case RIL_REQUEST_OEM_AIMS_RCS_PRESENCE:
    case RIL_REQUEST_OEM_AIMS_XCAP_MANAGE:
    case RIL_REQUEST_OEM_AIMS_RCS_CONFIG_MANAGE:
    case RIL_REQUEST_OEM_AIMS_RCS_TLS_MANAGE:
    case RIL_REQUEST_OEM_AIMS_AT_CMD_FORWARDING:
    case RIL_REQUEST_OEM_AIMS_CALL_TRANSFER:
    case RIL_REQUEST_OEM_AIMS_COMMON_IMS_SVC_INFO:
    case RIL_REQUEST_OEM_AIMS_COMPOSER_MMTEL:
    case RIL_REQUEST_OEM_AIMS_ENRICHED_CALLING:
    case RIL_REQUEST_OEM_AIMS_CANCEL_COMPOSER_MMTEL:
    case RIL_REQUEST_OEM_AIMS_CALL_DEFLECTION_REQ:
    case RIL_REQUEST_OEM_AIMS_SET_RTP_HEADER_EXTENSION_TYPE:
    case RIL_REQUEST_OEM_AIMS_SEND_RTP_HEADER_EXTENSION:
    case RIL_REQUEST_OEM_AIMS_SEND_UL_SIP_MSG_IND:
    case RIL_REQUEST_OEM_AIMS_DL_SIP_MSG_RESULT_IND:
    case RIL_REQUEST_OEM_AIMS_REGISTER_IND:
    case RIL_REQUEST_OEM_AIMS_SET_TEMP_EMERGENCY_NUMBER:
    case RIL_REQUEST_OEM_AIMS_DC_SESSION_UPDATE:
    case RIL_REQUEST_OEM_AIMS_DC_SESSION_MANAGE:
    case RIL_REQUEST_OEM_AIMS_DC_SESSION_MODIFY_IND:
    case RIL_REQUEST_OEM_AIMS_EMERGENCY_VOWIFI_CAPABILITY:
        rilsockid = GetProperSocketIdForOem(OEM_CAT_IMS);
        break;
    case RIL_REQUEST_OEM_WFC_MEDIA_CHANNEL_CONFIG:
    case RIL_REQUEST_OEM_WFC_DTMF_START:
        rilsockid = RIL_SOCKET_1;
        break;
    case RIL_REQUEST_OEM_SIM_OPEN_CHANNEL:
    case RIL_REQUEST_OEM_SIM_TRANSMIT_APDU_LOGICAL:
    case RIL_REQUEST_OEM_SIM_TRANSMIT_APDU_BASIC:
    case RIL_REQUEST_SIM_CLOSE_CHANNEL:
    case RIL_REQUEST_SIM_GET_ATR:
    case RIL_REQUEST_OEM_SIM_PRESENT:
    case RIL_REQUEST_OEM_SIM_RESET:
        rilsockid = GetProperSocketIdForOem(OEM_CAT_SIM);
        break;
    default:
        break;
    }

    return rilsockid;
}

void RilApplication::OnRequestComplete(RilContext *context, RIL_Token t, RIL_Errno e, void *response, unsigned int responselen) {
    if (context != NULL) {
        OnRequestComplete(t, e, response, responselen);
    }
}

void RilApplication::OnUnsolicitedResponse(RilContext *context, int unsolResponse, const void *data, unsigned int datalen) {
    if (context != NULL) {
        OnUnsolicitedResponse(unsolResponse, data, datalen, context->GetRilSocketId());
    }
}

void RilApplication::OnRequestAck(RIL_Token t) {
    RilLogI("[%s] %s", TAG, __FUNCTION__);
    if (t) {
        RilAppToken *token = static_cast<RilAppToken *>(t);
        if (token != NULL) {
            token->context->OnRequestAck(token->t);
        }
    }
}

// for unit test
void RilApplication::UpdateUnitTestSerial(RIL_Token t) {
    if (mUnitTestMode != 0) {
        if (t != NULL)  {
            RIL_TestToken *pTestToken = (RIL_TestToken *)t;
            mUnitTestReqSerial = pTestToken->serial;
        } else {
            mUnitTestReqSerial = -1;
        }
    }
}

void RilApplication::OnUpdateReqModemData(RIL_Token t, void *data, unsigned int len) {
    if (t) {
        RilAppToken *token = static_cast<RilAppToken *>(t);
        token->context->OnUpdateReqModemData(token->t, data, len);
        // token shall be deleted after processing response
        // delete token;
    }
}

void RilApplication::OnSitrilStateUpdate(int /* radioState */, int /* networkRegState */, int cardState) {
    RilContext *pRilContext = GetRilContext(RIL_SOCKET_1);
    if (pRilContext != NULL) {
        RilDataInts *rildata = new RilDataInts(2);
        if ((RIL_CardState)cardState == RIL_CARDSTATE_PRESENT) {    // Present
            rildata->SetInt(0, RIL_CARDSTATE_PRESENT);
            rildata->SetInt(1, RIL_APPSTATE_READY);
        } else {
            rildata->SetInt(0, RIL_CARDSTATE_ABSENT);
            rildata->SetInt(1, (int)-1);
        }
        pRilContext->BroadcastSystemMessage(MSG_SYSTEM_SIM_STATUS_CHANGED, rildata);
    }

    RilProperty *property = GetProperty();
    // overallCardState
    // bit 0 : SIM1 card state
    // bit 1 : SIM2 card state
    int overallCardState = property->GetInt(RIL_SIM_OVERALL_CARDSTATE, 0);
    if (cardState == RIL_CARDSTATE_PRESENT) {
        overallCardState |= (0x01 << RIL_SOCKET_1);
    } else {
        overallCardState &= ~(0x01 << RIL_SOCKET_1);
    }
    property->Put(RIL_SIM_OVERALL_CARDSTATE, overallCardState);
}

void RilApplication::OnModemRspSend(void *data, unsigned int datalen) {
    if (m_RilContext[0] != NULL && data != NULL && datalen > 0) {
        m_RilContext[0]->ProcessModemData(data, datalen);
    }
}

int RilApplication::UpdateReqModemData(RIL_Token t, void *data, unsigned int len) {
    int ret = -1;
    if (t) {
        RilAppToken *token = static_cast<RilAppToken *>(t);
        if (token != NULL && token->context != NULL && token->t != NULL) {
            RIL_TestToken *pTestToken = (RIL_TestToken *)(token->t);
            int serial = pTestToken->serial;
            if (mUnitTestReqSerial == serial) {
                OnUpdateReqModemData(t, data, len);
                ret = 0;
            }
        }
    }
    return ret;
}

/* SAP */
void RilApplication::SetSapEnv(const struct RIL_Env *pSapEnv) {
    m_pSapEnv = pSapEnv;
    if (m_pSapRespListener) {
        delete m_pSapRespListener;
    }
    m_pSapRespListener = new SapResponseListener(pSapEnv);
    m_pSapRespListener->SetRilApplication(this);
}

void RilApplication::OnSapRequest(int request, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id) {
    UpdateUnitTestSerial(t);

    RilContext *pRilContext = GetRilContext(socket_id);
    if (pRilContext != NULL) {
        RilAppToken *token = new RilAppToken(m_pSapRespListener, t);
        // TODO need to request ID mapping for compatibility of RIL Requests
        if (request > 0) {
            request += RIL_REQUEST_SAP_BASE;
        }
        pRilContext->OnRequest(request, data, datalen, (RIL_Token)token);
    }
    else {
        RilLogW("[%s] Invalid RIL_SOCKET_ID %d", TAG, socket_id);
    }
}

void RilApplication::OnSapRequest(int request, void *data, unsigned int datalen, RIL_Token t) {
    OnSapRequest(request, data, datalen, t, RIL_SOCKET_1);
}

/* RIL vendor external */
void RilApplication::externalOnRequest(int reqOemId, void *data, unsigned int datalen, RIL_Token t, RIL_SOCKET_ID socket_id) {
    RilLogV("[%s] %s reqOemId=%d, t=%p, socketid=%d%s",
            TAG, __FUNCTION__, reqOemId, t, socket_id, (socket_id==RIL_SOCKET_UNKNOWN?"(unknown)":""));

    UpdateUnitTestSerial(t);

    if (socket_id == RIL_SOCKET_UNKNOWN) {
        //RadioExternalRequestInfo* p_reqInfo = (RadioExternalRequestInfo *)t;
        socket_id = GetProperSocketId(reqOemId, data, datalen);
        //if(p_reqInfo != NULL) p_reqInfo->slotId = socket_id;
        RilLogV("[%s] found proper socketid=%d", TAG, socket_id);
    }

    RilContext *pRilContext = GetRilContext(socket_id);
    if (pRilContext != NULL) {
        RilAppToken *token = new RilAppToken(m_pRilExternalRespListener, t);
        pRilContext->OnRequest(reqOemId, data, datalen, (RIL_Token)token);
    }
}

void RilApplication::setRilExternalEnv(const struct RIL_External_Env *pRilExtEnv) {
    m_pRilExternalEnv = pRilExtEnv;
    if (m_pRilExternalRespListener) {
        delete m_pRilExternalRespListener;
    }
    m_pRilExternalRespListener = new RilExternalResponseListener(pRilExtEnv);
}

// multi-sim API
bool RilApplication::IsMultiSimEnabled()
{
    return (RilApplication::GetPhoneCount() > 1);
}

int RilApplication::GetPhoneCount() {
    // multi-sim mode: single or ds
    int phoneCount = 1;  // default as single
    string config = SystemProperty::Get("persist.radio.multisim.config");
    if (TextUtils::Equals(config, "dsds") ||
        TextUtils::Equals(config, "dsda") ||
        TextUtils::Equals(config, "tsts")) {
        phoneCount = 2;
    }
    return phoneCount;
}
