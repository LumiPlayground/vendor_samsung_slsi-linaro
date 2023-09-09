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
 * servicehandlerfactory_test.cpp
 *
 *  Created on: 2021. 7. 13.
 */
#include <gtest/gtest.h>
#include "audio/audiohandlerfactory.h"
#include "cs/cschandlerfactory.h"
#include "ims/imshandlerfactory.h"
#include "ims/apimshandlerfactory.h"
#include "misc/deviceconfighandlerfactory.h"
#include "misc/mischandlerfactory.h"
#include "misc/oemmischandlerfactory.h"
#include "network/networkhandlerfactory.h"
#include "network/miscnetworkhandlerfactory.h"
#include "radioconfig/radioconfighandlerfactory.h"
#include "sap/saphandlerfactory.h"
#include "sms/smshandlerfactory.h"
#include "supp/supphandlerfactory.h"
#include "uicc/uicchandlerfactory.h"
#include "vsim/vsimhandlerfactory.h"
#include "service.h"
#include <librilutils/textutils.h>

#include <set>

//#define DLOG(x, ...) printf(x"\n", ##__VA_ARGS__)

#define TEST_SERVICE_HANDLER_FACTORY(theClassName) \
class Test_##theClassName : public theClassName { \
public: \
    Test_##theClassName() : theClassName() { theClassName::OnLoad(0, nullptr); } \
    virtual ~Test_##theClassName() {} \
    static Test_##theClassName *GetInstance() { return new Test_##theClassName(); } \
}; \

TEST_SERVICE_HANDLER_FACTORY(AudioHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(CscHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(ImsHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(ApImsHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(DeviceConfigHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(MiscHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(OemMiscHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(NetworkHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(MiscNetworkHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(RadioConfigHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(SapHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(SmsHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(SuppHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(UiccHandlerFactory)
TEST_SERVICE_HANDLER_FACTORY(VsimHandlerFactory)

#define TEST_SERVICE_HANDLER_FACTORY_INSTANCE(testSet, theClassName) \
testSet.insert(Test_##theClassName::GetInstance()); \

TEST(ServiceHandlerFactory, factory_create_destroy) {
    std::set<ServiceHandlerFactory *> factories;
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, AudioHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, CscHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, ImsHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, ApImsHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, DeviceConfigHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, MiscHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, OemMiscHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, NetworkHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, RadioConfigHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, SapHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, SmsHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, SuppHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, UiccHandlerFactory)
    TEST_SERVICE_HANDLER_FACTORY_INSTANCE(factories, VsimHandlerFactory)

    for (auto& f : factories) {
        ASSERT_NE(f, nullptr);
        EXPECT_TRUE(!TextUtils::IsEmpty(f->GetName()));
        delete f;
    }
    factories.clear();
}


class TestService : public Service {
public:
    TestService() : Service(nullptr, 1000) {}
    virtual ~TestService() {}
};

void testServiceHandlerFactoryGetHandler(ServiceHandlerFactory& f,
        int testVector[], size_t size) {
    TestService testService;
    for (size_t i = 0; i < size; i++) {
        auto h = f.GetHandler(testVector[i], &testService);
        ASSERT_NE(h, nullptr);
        EXPECT_TRUE(!TextUtils::IsEmpty(h->GetName()));
        Message testMessage {};
        EXPECT_FALSE(h->OnHandleRequest(&testMessage));
        EXPECT_FALSE(h->OnHandleSolicitedResponse(&testMessage));
        EXPECT_FALSE(h->OnHandleUnsolicitedResponse(&testMessage));
        EXPECT_FALSE(h->OnHandleRequestTimeout(&testMessage));
        EXPECT_FALSE(h->OnHandleInternalMessage(&testMessage));
        delete h;
    }

    EXPECT_EQ(f.GetHandler(-1, &testService), nullptr);
    EXPECT_EQ(f.GetHandler(testVector[0], nullptr), nullptr);
}

TEST(ServiceHandlerFactory, AudioHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_AUDIO_SET_VOLUME,
        MSG_AUDIO_GET_VOLUME,
        MSG_AUDIO_SET_AUDIOPATH,
        MSG_AUDIO_GET_AUDIOPATH,
        MSG_AUDIO_SET_MULTIMIC,
        MSG_AUDIO_GET_MULTIMIC,
        MSG_AUDIO_SET_AUDIO_CLOCK,
        MSG_AUDIO_SET_AUDIO_LOOPBACK,
        MSG_CS_SOUND_SET_MUTE,
        MSG_CS_SOUND_GET_MUTE,
        MSG_CS_SOUND_SWITCH_VOICE_CALL,
        MSG_CS_IND_RESEND_IN_CALL_MUTE,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_AudioHandlerFactory f;

    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, CscHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_CS_CALL_LIST,
        MSG_CS_CALL_DIAL,
        MSG_CS_CALL_EMERGENCY_DIAL,
        MSG_CS_CALL_ANSWER,
        MSG_CS_SS_EXPLICIT_CALL_TRANSFER,
        MSG_CS_CALL_HANGUP,
        MSG_CS_LAST_CALL_FAIL_CAUSE,
        MSG_CS_SOUND_RINGBACKTONE_NTF,
        MSG_CS_CALL_STATE_CHANGE_NTF,
        MSG_CS_CALL_RINGING_NTF,
        MSG_CS_IND_EMERGENCY_SUPPORT_RAT_MODE,
        MSG_NET_EMERGENCY_CALL_LIST_RECEIVED,
        MSG_CS_SS_UDUB,
        MSG_CS_SS_HANGUP_FOREGROUND_RESUME_BACKGROUND,
        MSG_CS_SS_HANGUP_WAITING_OR_BACKGROUND,
        MSG_CS_SS_SWITCH_WAITING_OR_HOLDING_AND_ACTIVE,
        MSG_CS_SS_CONFERENCE,
        MSG_CS_SS_SEPARATE_CONNECTION,
        MSG_CS_SEND_CALL_CONFIRM,
        MSG_CS_CALL_PRESENT_NTF,
        MSG_CS_IND_ENTER_EMERGENCY_CB_MODE_NTF,
        MSG_CS_IND_EXIT_EMERGENCY_CB_MODE_NTF,
        MSG_CS_EXIT_EMERGENCY_CB_MODE,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_CscHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, ImsHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_AIMS_DIAL,
        MSG_AIMS_DEREGISTRATION,
        MSG_AIMS_CALL_MANAGE,
        MSG_AIMS_CALL_MODIFY,
        MSG_AIMS_RESPONSE_CALL_MODIFY,
        MSG_AIMS_CONF_CALL_ADD_REMOVE_USER,
        MSG_AIMS_ENHANCED_CONF_CALL,
        MSG_AIMS_GET_CALL_FORWARD_STATUS,
        MSG_AIMS_SET_CALL_FORWARD_STATUS,
        MSG_AIMS_GET_CALL_WAITING,
        MSG_AIMS_GET_CALL_BARRING,
        MSG_AIMS_SET_CALL_BARRING,
        MSG_AIMS_CHG_BARRING_PWD,
        MSG_AIMS_SEND_USSD_INFO,
        MSG_AIMS_HANGUP,
        MSG_AIMS_HIDDEN_MENU,
        MSG_AIMS_SEND_DTMF,
        MSG_AIMS_SET_FRAME_TIME,
        MSG_AIMS_GET_FRAME_TIME,
        MSG_AIMS_TIME_INFO,
        MSG_AIMS_GET_PRESENTATION_SETTINGS,
        MSG_AIMS_SET_PRESENTATION_SETTINGS,
        MSG_AIMS_SET_SELF_CAPABILITY,
        MSG_AIMS_XCAPM_START_REQ,
        MSG_AIMS_XCAPM_STOP_REQ,
        MSG_AIMS_RTT_SEND_TEXT,
        MSG_AIMS_EXIT_EMERGENCY_CB_MODE,
        MSG_AIMS_SET_GEO_LOCATION_INFO,
        MSG_AIMS_CDMA_SEND_SMS,
        MSG_AIMS_RCS_MULTI_FRAME_REQ,
        MSG_AIMS_RCS_CHAT_REQ,
        MSG_AIMS_RCS_GROUP_CHAT_REQ,
        MSG_AIMS_RCS_OFFLINE_MODE_REQ,
        MSG_AIMS_RCS_FILE_TRANSFER_REQ,
        MSG_AIMS_RCS_COMMON_MESSAGE_REQ,
        MSG_AIMS_RCS_CONTENT_SHARE_REQ,
        MSG_AIMS_RCS_PRESENCE_REQ,
        MSG_AIMS_XCAP_MANAGE_REQ,
        MSG_AIMS_RCS_CONFIG_MANAGE_REQ,
        MSG_AIMS_RCS_TLS_MANAGE_REQ,
        MSG_AIMS_SET_PDN_EST_STATUS,
        MSG_AIMS_SET_RTP_RX_STATISTICS,
        MSG_WFC_MEDIA_CHANNEL_CONFIG,
        MSG_WFC_DTMF_START,
        MSG_WFC_SET_VOWIFI_HO_THRESHOLD,
        MSG_AIMS_AT_CMD_FORWARDING,
        MSG_AIMS_CALL_TRANSFER,
        MSG_AIMS_COMMON_IMS_SVC_INFO,
        MSG_AIMS_COMPOSER_MMTEL_REQ,
        MSG_AIMS_ENRICHED_CALLING,
        MSG_AIMS_CANCEL_COMPOSER_MMTEL,
        MSG_AIMS_CALL_DEFLECTION_REQ,
        MSG_AIMS_SET_RTP_HEADER_EXTENSION_TYPE,
        MSG_AIMS_SEND_RTP_HEADER_EXTENSION,
        MSG_AIMS_HO_TO_WIFI_READY,
        MSG_AIMS_HO_TO_3GPP,
        MSG_AIMS_ADD_PDN_INFO,
        MSG_MISC_AIMS_SEND_SMS_ACK,
        MSG_MISC_AIMS_SEND_ACK_INCOMING_SMS,
        MSG_MISC_AIMS_SEND_ACK_INCOMING_CDMA_SMS,
        MSG_AIMS_DEL_PDN_INFO,
        MSG_AIMS_STACK_START_REQ,
        MSG_AIMS_STACK_STOP_REQ,
        MSG_AIMS_SET_HIDDEN_MENU_ITEM,
        MSG_AIMS_GET_HIDDEN_MENU_ITEM,
        MSG_AIMS_HO_TO_WIFI_CANCEL_IND,
        MSG_AIMS_PAYLOAD_INFO_IND,
        MSG_AIMS_MEDIA_STATE_IND,
        MSG_AIMS_SEND_UL_SIP_MSG_IND,
        MSG_AIMS_DL_SIP_MSG_RESULT_IND,
        MSG_AIMS_REGISTER_IND,
        MSG_AIMS_DC_SESSION_UPDATE,
        MSG_AIMS_DC_SESSION_MANAGE,
        MSG_AIMS_DC_SESSION_MODIFY_IND,
        MSG_IND_AIMS_CALL_RING,
        MSG_IND_AIMS_CALL_STATUS,
        MSG_IND_AIMS_CALL_MODIFY,
        MSG_IND_AIMS_EXPIRE_INFO,
        MSG_IND_AIMS_FRAME_TIME,
        MSG_IND_AIMS_SUPP_SVC_NOTIFICATION,
        MSG_IND_AIMS_NEW_SMS,
        MSG_IND_AIMS_NEW_SMS_STATUS_REPORT,
        MSG_IND_AIMS_ON_USSD,
        MSG_IND_AIMS_CONFERENCE_CALL_EVENT,
        MSG_IND_AIMS_PAYLOAD_INFO,
        MSG_IND_AIMS_VOWIFI_HO_CALL_INFO,
        MSG_IND_AIMS_NEW_CDMA_SMS,
        MSG_IND_AIMS_RINGBACK_TONE,
        MSG_IND_AIMS_CALL_MANAGE,
        MSG_IND_AIMS_CONF_CALL_ADD_REMOVE_USER,
        MSG_IND_AIMS_ENHANCED_CONF_CALL,
        MSG_IND_AIMS_CALL_MODIFY_RSP,
        MSG_IND_AIMS_DTMF_EVENT,
        MSG_IND_AIMS_RTT_NEW_TEXT,
        MSG_IND_AIMS_RTT_FAIL_SENDING_TEXT,
        MSG_IND_AIMS_EXIT_EMERGENCY_CB_MODE,
        MSG_IND_AIMS_DIALOG_INFO,
        MSG_IND_AIMS_RCS_MULTI_FRAME,
        MSG_IND_AIMS_RCS_CHAT,
        MSG_IND_AIMS_RCS_GROUP_CHAT,
        MSG_IND_AIMS_RCS_OFFLINE_MODE,
        MSG_IND_AIMS_RCS_FILE_TRANSFER,
        MSG_IND_AIMS_RCS_COMMON_MESSAGE,
        MSG_IND_AIMS_RCS_CONTENT_SHARE,
        MSG_IND_AIMS_RCS_PRESENCE,
        MSG_IND_AIMS_RCS_XCAP_MANAGE,
        MSG_IND_AIMS_RCS_CONFIG_MANAGE,
        MSG_IND_AIMS_RCS_TLS_MANAGE,
        MSG_IND_AIMS_MEDIA_STATUS,
        MSG_IND_AIMS_SIP_MSG_INFO,
        MSG_IND_AIMS_VOICE_RTQ_QUALITY,
        MSG_IND_AIMS_RTP_RX_STATISTICS,
        MSG_IND_AIMS_AT_CMD_FORWARDING,
        MSG_IND_AIMS_FEAT_SUPP,
        MSG_IND_AIMS_CALL_TRANSFER_RSP,
        MSG_IND_AIMS_CALL_TRANSFER,
        MSG_IND_AIMS_MWI_INFO,
        MSG_IND_AIMS_COMPOSER_MMTEL,
        MSG_IND_AIMS_ENRICHED_CALLING,
        MSG_IND_AIMS_RTP_HEADER_EXTENSION_TYPE,
        MSG_IND_AIMS_RTP_HEADER_EXTENSION,
        MSG_IND_AIMS_SIP_CONFIG,
        MSG_IND_AIMS_UL_SIP_MSG_RESULT,
        MSG_IND_AIMS_SEND_DL_SIP_MSG,
        MSG_IND_AIMS_DC_SESS_UPDATE,
        MSG_IND_AIMS_DC_SESS_MANAGE,
        MSG_IND_AIMS_DC_SESS_MODIFY,
        MSG_IND_WFC_RTP_RTCP_TIMEOUT,
        MSG_IND_WFC_FIRST_RTP,
        MSG_IND_WFC_RTCP_RX_SR,
        MSG_IND_WFC_RCV_DTMF_NOTI,
        MSG_AIMS_EMERGENCY_VOWIFI_CAPABILITY,
        MSG_IND_AIMS_CODEC_INFO,
        MSG_IMS_MEDIA_IND_MAC_CE_INFO,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_ImsHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, DeviceConfigHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_MISC_BASEBAND_VER,
        MSG_MISC_GET_IMEI,
        MSG_MISC_GET_IMEISV,
        MSG_MISC_DEV_IDENTITY,
        MSG_MISC_OEM_SET_ENG_MODE,
        MSG_MISC_OEM_SET_SCREEN_LINE,
        MSG_MISC_OEM_SET_ENG_STRING_INPUT,
        MSG_MISC_UNSOL_OEM_DISPLAY_ENG,
        MSG_MISC_OEM_SET_DEBUG_TRACE,
        MSG_MISC_OEM_SEND_SGC,
        MSG_MISC_SET_DEVICE_INFO,
        MSG_MISC_SET_OPEN_CARRIER_INFO,
        MSG_MISC_GET_HARDWARE_CONFIG,
        MSG_MISC_IND_HARDWARE_CONFIG_CHANGED,
        MSG_MISC_SET_CARRIER_INFO_IMSI_ENCRYPTION,
        MSG_MISC_IND_CARRIER_INFO_IMSI_ENCRYPTION,
        MSG_MISC_SET_SIGNAL_STRENGTH_REPORTING_CRITERIA,
        MSG_MISC_SET_LINK_CAPACITY_REPORTING_CRITERIA,
        MSG_MISC_OEM_RADIO_CONFIG_RESET,
        MSG_MISC_GET_PLMN_NAME_FROM_SE13TABLE,
        MSG_MISC_TS25TABLE_DUMP,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_DeviceConfigHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, MiscHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_MISC_SCREEN,
        MSG_MISC_SEND_DEVICE_STATE,
        MSG_MISC_DTMF,
        MSG_MISC_DTMF_START,
        MSG_MISC_DTMF_STOP,
        MSG_MISC_IND_MODEM_RESTART,
        MSG_MISC_SET_INDICATION_FILTER,
        MSG_MISC_FORWARDING_AT_COMMAND,
        MSG_MISC_UNSOL_AT_COMMAND,
        MSG_MISC_PHONE_RESET,
        MSG_MISC_CDMA_GET_SUBSCRIPT_SOURCE,
        MSG_MISC_CDMA_SET_SUBSCRIPT_SOURCE,
        MSG_MISC_GET_CDMA_SUBSCRIPTION,
        MSG_MISC_IND_CDMA_PRL_CHANGED,
        MSG_MISC_SET_PSENSOR_STATUS,
        MSG_MISC_SET_SAR_STATE,
        MSG_MISC_GET_SAR_STATE,
        MSG_MISC_SAR_CONTROL_STATE_IND,
        MSG_MISC_SAR_RF_CONNECTION_IND,
        MSG_MISC_RF_CONNECTION_STATE_IND,
        MSG_MISC_SCAN_RSSI,
        MSG_MISC_SCAN_RSSI_RESULT_RECEIVED,
        MSG_MISC_SET_FEATURE_INFO,
        MSG_MISC_QUERY_TTY,
        MSG_AUDIO_SET_TTY_MODE,
        MSG_MISC_SET_TTY,
        MSG_MISC_UNSOL_SELFLOG_STATUS,
        MSG_MISC_SET_ACTIVATE_VSIM,
        MSG_MISC_SET_MODEM_LOG_DUMP,
        MSG_MISC_SET_AP_SYSTEM_TIME,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_MiscHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, OemMiscHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_MISC_OEM_SET_PIN_CONTROL,
        MSG_MISC_UNSOL_OEM_PIN_CONTROL,
        MSG_MISC_OEM_GET_RADIO_NODE,
        MSG_MISC_OEM_SET_RADIO_NODE,
        MSG_MISC_OEM_GET_PROVISION_UPDATE_REQUEST,
        MSG_MISC_OEM_SET_PROVISION_UPDATE_DONE_REQUEST,
        MSG_MISC_OEM_GET_MSL_CODE,
        MSG_MISC_OEM_GET_RF_DESENSE_MODE,
        MSG_MISC_OEM_SET_RF_DESENSE_MODE,
        MSG_MISC_OEM_GET_MANUAL_BAND_MODE,
        MSG_MISC_OEM_SET_MANUAL_BAND_MODE,
        MSG_MISC_OEM_MODEM_INFO,
        MSG_MISC_OEM_MODEM_INFO_RECEIVED,
        MSG_MISC_OEM_MODEM_RESET,
        MSG_MISC_OEM_SET_FUNC_SWITCH_REQ,
        MSG_MISC_OEM_SET_PDCP_DISCARD_TIMER,
        MSG_MISC_OEM_SET_SELFLOG,
        MSG_MISC_OEM_SET_SELFLOG_PROFILE,
        MSG_MISC_OEM_GET_SELFLOG_STATUS,
        MSG_MISC_OEM_GET_CQI_INFO,
        MSG_MISC_OEM_SET_SAR_SETTING,
        MSG_MISC_OEM_SET_GMO_SWITCH,
        MSG_MISC_OEM_SET_TCS_FCI_REQ,
        MSG_MISC_OEM_GET_TCS_FCI_INFO,
        MSG_MISC_OEM_SET_CA_BW_FILTER,
        MSG_MISC_OEM_CA_BW_FILTER_IND,
        MSG_MISC_OEM_SET_ELEVATOR_SENSOR,
        MSG_MISC_OEM_IND_NTW_ENDC_CAPABILITY,
        MSG_MISC_OEM_SET_FORBID_LTE_CELL,
        MSG_MISC_OEM_IND_BASESTATION_INFO,
        MSG_MISC_OEM_GET_MODEM_ACTIVITY_INFO,
        MSG_MISC_OEM_SET_SVN,
        MSG_MISC_OEM_IND_DROPPED_MT_CALL_INFO,
        MSG_MISC_OEM_SET_FORBID_FREQUENCY,
        MSG_MISC_OEM_SET_FORBID_LTE_BAND,
        MSG_MISC_OEM_SET_FORBID_LTE_TAC,
        MSG_MISC_OEM_SET_SELFLOG_TCP_DUMP_MODE,
        MSG_MISC_OEM_SET_ALL_DIV_SETTING,
        MSG_MISC_OEM_SET_BSR_PARA,
        MSG_MISC_OEM_SET_AHEAD_RRE,
        MSG_MISC_OEM_SET_DRDV,
        MSG_MISC_OEM_SET_EMC_LAB_TEST_MODE,
        MSG_MISC_OEM_SET_FORBID_NR_CELL,
        MSG_MISC_OEM_GET_CDMA_CAPABILITY_INFO,
        MSG_MISC_OEM_TEST_UNSOL,
        MSG_MISC_OEM_UPDATE_EMERGENCY_NUMBER_LIST,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_OemMiscHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, NetworkHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_NET_VOICE_REGISTRATION_STATE,
        MSG_NET_DATA_REGISTRATION_STATE,
        MSG_NET_NETWORK_STATE_CHANGED,
        MSG_NET_OPERATOR,
        MSG_NET_RADIO_POWER,
        MSG_NET_GET_RADIO_STATE,
        MSG_NET_RADIO_STATE_CHANGED,
        MSG_NET_SHUTDOWN,
        MSG_NET_VOICE_RADIO_TECH,
        MSG_NET_SET_BAND_MODE,
        MSG_NET_QUERY_AVAILABLE_BAND_MODE,
        MSG_NET_ALLOW_DATA,
        MSG_NET_SET_UNSOL_CELL_INFO_LIST_RATE,
        MSG_NET_GET_DUPLEX_MODE,
        MSG_NET_SET_DUPLEX_MODE,
        MSG_NET_QUERY_NETWORK_SELECTION_MODE,
        MSG_NET_SET_NETWORK_SELECTION_AUTO,
        MSG_NET_GET_CELL_INFO_LIST,
        MSG_NET_CELL_INFO_LIST_RECEIVED,
        MSG_NET_SET_DS_NTW_TYPE,
        MSG_NET_GET_PREF_NETWORK_TYPE,
        MSG_NET_SET_PREF_NETWORK_TYPE,
        MSG_NET_SET_DUAL_NTW_AND_ALLOW_DATA,
        MSG_NET_START_NETWORK_SCAN,
        MSG_NET_STOP_NETWORK_SCAN,
        MSG_NET_IND_NETWORK_SCAN_RESULT,
        MSG_NET_OEM_SET_ENDC_MODE,
        MSG_NET_OEM_GET_ENDC_MODE,
        MSG_NET_SET_NR_MODE,
        MSG_NET_GET_NR_MODE,
        MSG_NET_SET_NR_SILENCE_MODE,
        MSG_NET_GET_NR_SILENCE_MODE,
        MSG_NET_OEM_SET_PS_SERVICE,
        MSG_NET_OEM_GET_PS_SERVICE,
        MSG_NET_SET_MC_SRCH,
        MSG_NET_GET_RC_NTW_TYPE,
        MSG_NET_SET_RC_NTW_TYPE,
        MSG_NET_RC_INFO_RECV,
        MSG_NET_IND_TOTAL_OOS,
        MSG_NET_IND_MCC,
        MSG_NET_SET_SYSTEM_SELECTION_CHANNELS,
        MSG_NET_GET_BARRING_INFO,
        MSG_NET_IND_BARRING_INFO,
        MSG_NET_IND_REG_FAILED,
        MSG_NET_QUERY_AVAILABLE_EMERGENCY_CALL_STATUS,
        MSG_NET_SET_EMERGENCY_CALL_STATUS,
        MSG_NET_EMERGENCY_ACT_INFO_RECEIVED,
        MSG_NET_SET_ALLOWED_NETWORK,
        MSG_NET_GET_ALLOWED_NETWORK,
        MSG_NET_SET_PLMN_LIST,
        MSG_NET_GET_PLMN_LIST,
        MSG_NET_SET_IMS_REGISTRATION_STATE,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_NetworkHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, MiscNetworkHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_MISC_OEM_CANCEL_AVAILABLE_NETWORKS,
        MSG_MISC_OEM_SET_PREFERRED_CALL_CAPABILITY,
        MSG_MISC_OEM_GET_PREFERRED_CALL_CAPABILITY,
        MSG_MISC_SET_VOICE_OPERATION,
        MSG_MISC_GET_ACTIVITY_INFO,
        MSG_MISC_LCE_START,
        MSG_MISC_LCE_STOP,
        MSG_MISC_PULL_LCEDATA,
        MSG_MISC_IND_CURRENT_LINK_CAPACITY_ESTIMATE,
        MSG_MISC_GET_NEIGHBORING_CELL_IDS,
        MSG_MISC_SET_LOCATION_UPDATES,
        MSG_MISC_SET_SUPP_SVC_NOTIFICATION,
        MSG_MISC_ENABLE_MODEM,
        MSG_MISC_GET_MODEM_STACK_STATUS,
        MSG_MISC_UNSOL_NITZ_TIME_RECEIVED,
        MSG_NET_OEM_GET_IMS_SUPPORT_SERVICE,
        MSG_NET_IND_SIM_FILE_INFO,
        MSG_MISC_SET_DATA_THROTTLING,
        MSG_NET_GET_SYSTEM_SELECTION_CHANNELS,
        MSG_MISC_CDMA_GET_SUBSCRIPT_SOURCE,
        MSG_MISC_CDMA_SET_SUBSCRIPT_SOURCE,
        MSG_MISC_GET_CDMA_SUBSCRIPTION,
        MSG_MISC_IND_CDMA_PRL_CHANGED,
        MSG_NET_CDMA_SET_ROAMING,
        MSG_NET_CDMA_QUERY_ROAMING,
        MSG_NET_SET_CDMA_HYBRID_MODE,
        MSG_NET_GET_CDMA_HYBRID_MODE,
        MSG_CS_CDMA_BURST_DTMF,
        MSG_CS_CDMA_SET_PREFERRED_VOICE_PRIVACY_MODE,
        MSG_CS_CDMA_QUERY_PREFERRED_VOICE_PRIVACY_MODE,
        MSG_CS_IND_CDMA_INFO_REC,
        MSG_CS_SS_IND_CDMA_CALL_WAITING,
        MSG_CS_SS_CDMA_FLASH,
        MSG_CS_IND_CDMA_OTA_PROVISION_STATUS,
        MSG_NET_OEM_GET_MANUAL_RAT_MODE,
        MSG_NET_OEM_SET_MANUAL_RAT_MODE,
        MSG_NET_OEM_GET_FREQUENCY_LOCK,
        MSG_NET_OEM_SET_FREQUENCY_LOCK,
        MSG_NET_GET_FREQUENCY_INFO,
        MSG_NET_IND_FREQUENCY_INFO,
        MSG_NET_IND_B2_B1_CONFIG,
        MSG_NET_IND_AC_BARRING_INFO,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_MiscNetworkHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, RadioConfigHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_MISC_GET_PHONE_CAPABILITY,
        MSG_MISC_SET_MODEMS_CONFIG,
        MSG_SIM_GET_SLOT_STATUS,
        MSG_SIM_SLOT_STATUS_CHANGED,
        MSG_SIM_SET_LOGICAL_TO_PHYSICAL_SLOT_MAPPING,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_RadioConfigHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, SapHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_SAP_CONNECT,
        MSG_SAP_DISCONNECT,
        MSG_SAP_APDU,
        MSG_SAP_TRANSFER_ATR,
        MSG_SAP_POWER,
        MSG_SAP_RESET_SIM,
        MSG_SAP_TRANSFER_CARD_READER_STATUS,
        MSG_SAP_SET_TRANSFER_PROTOCOL,
        MSG_SAP_DISCONNECT_IND,
        MSG_SAP_STATUS_IND,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_SapHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, SmsHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_SMS_SEND,
        MSG_SMS_SEND_MORE,
        MSG_SMS_WRITE_SMS_TO_SIM,
        MSG_SMS_DELETE_SMS_ON_SIM,
        MSG_SMS_GET_BROADCAST_SMS_CONFIG,
        MSG_SMS_SET_BROADCAST_SMS_CONFIG,
        MSG_SMS_BROADCAST_ACTIVATION,
        MSG_SMS_GET_SMSC_ADDRESS,
        MSG_SMS_SET_SMSC_ADDRESS,
        MSG_SMS_REPORT_SMS_MEMORY_STATUS,
        MSG_SMS_GET_STORAGE_CAPACITY,
        MSG_SMS_INCOMING_NEW_SMS,
        MSG_SMS_INCOMING_NEW_SMS_STATUS_REPORT,
        MSG_SMS_INCOMING_NEW_SMS_ON_SIM,
        MSG_SMS_SIM_SMS_STORAGE_FULL,
        MSG_SMS_INCOMING_NEW_BROADCAST_SMS,
        MSG_MISC_SMS_ACKNOWLEDGE,
        MSG_MISC_SMS_ACK_WITH_PDU,
        MSG_MISC_SMS_CDMA_ACKNOWLEDGE,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_SmsHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, SuppHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_SS_QUERY_CALL_WAITING,
        MSG_SS_SET_CALL_WAITING,
        MSG_SS_QUERY_CALL_FORWARDING,
        MSG_SS_SET_CALL_FORWARDING,
        MSG_SS_CALL_BARRING_PWD,
        MSG_SS_GET_CLIR,
        MSG_SS_SET_CLIR,
        MSG_SS_GET_CLIP,
        MSG_SS_QUERY_COLR,
        MSG_SS_QUERY_COLP,
        MSG_SS_SEND_USSD,
        MSG_SS_CANCEL_USSD,
        MSG_SS_USSD_NTF,
        MSG_SS_SVC_NTF,
        MSG_SS_IND,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_SuppHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, UiccHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_SIM_ENABLE_UICC_APP,
        MSG_SIM_GET_UICC_APP_ENABLEMENT,
        MSG_SIM_SET_UICC,
        MSG_SIM_GET_FACILITY_LOCK,
        MSG_SIM_GET_ISIM_AUTH,
        MSG_SIM_GET_SIM_AUTH,
        MSG_SIM_GET_ATR,
        MSG_SIM_OPEN_CHANNEL,
        MSG_SIM_CLOSE_CHANNEL,
        MSG_SIM_TRANSMIT_APDU_CHANNEL,
        MSG_SIM_TRANSMIT_APDU_BASIC,
        MSG_SIM_SET_CARRIER_RESTRICTIONS,
        MSG_SIM_GET_CARRIER_RESTRICTIONS,
        MSG_SIM_SET_SIM_CARD_POWER,
        MSG_SIM_IND_UICC_SUBSCRIPTION_STATUS_CHANGED,
        MSG_SIM_STATUS_CHANGED,
        MSG_SIM_DEPERSONALIZATION,
        MSG_SIM_GET_SIM_LOCK_INFO,
        MSG_SIM_OEM_OPEN_CHANNEL,
        MSG_SIM_OEM_TRANSMIT_APDU_LOGICAL,
        MSG_SIM_OEM_GET_CARD_PRESENT,
        MSG_SIM_OEM_SE_RESET,
        MSG_SIM_OEM_ICC_DEPERSONALIZATION,
        MSG_SAT_WIFI_CHANNEL_STATUS_REPORT,
        MSG_SAT_WIFI_STATUS_RESPONSE,
        MSG_SAT_WIFI_OPEN_CHANNEL_RESPONSE,
        MSG_SAT_WIFI_SEND_DATA_RESPONSE,
        MSG_SAT_WIFI_RECEIVE_DATA_RESPONSE,
        MSG_SAT_WIFI_CLOSE_CHANNEL_RESPONSE,
        MSG_SAT_IND_WIFI_STATUS_REQUEST,
        MSG_SAT_IND_WIFI_OPEN_CHANNEL_REQUEST,
        MSG_SAT_IND_WIFI_SEND_DATA_REQUEST,
        MSG_SAT_IND_WIFI_RECEIVE_DATA_REQUEST,
        MSG_SAT_IND_WIFI_CLOSE_CHANNEL_REQUEST,
        MSG_IND_SIM_TRAY_STATUS,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_UiccHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, VsimHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_VSIM_NOTIFICATION,
        MSG_VSIM_OPERATION,
        MSG_VSIM_OPERATION_IND,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_VsimHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}

TEST(ServiceHandlerFactory, ApImsHandlerFactory_GetHandler) {
    int testVector[] = {
        MSG_IMS_UPDATE_IMS_REGISTRATION_INFO,
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    Test_ApImsHandlerFactory f;
    testServiceHandlerFactoryGetHandler(f, testVector, size);
}
