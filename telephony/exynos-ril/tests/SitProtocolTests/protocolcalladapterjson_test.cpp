/*
 * Copyright (c) 2021 Samsung Electronics Co., Ltd All Rights Reserved
 * PROPRIETARY/CONFIDENTIAL
 *
 * This software is the confidential and proprietary information of
 * SAMSUNG ELECTRONICS ("Confidential Information").
 *
 * You shall not disclose such Confidential Information and shall use it
 * only in accordance with the terms of the license agreement
 * you entered into with SAMSUNG ELECTRONICS.
 *
 * SAMSUNG make no representations or warranties about the suitability of
 * the software, either express or implied, including but not limited to
 * the implied warranties of merchantability, fitness for a particular
 * purpose, or non-infringement. SAMSUNG shall not be liable for any
 * damages suffered by license as a result of using, modifying or
 * distributing this software or its derivatives.
 */
#include <gtest/gtest.h>

#include "modemdatabuilder.h"
#include "modemdata.h"
#include "json/cs/protocolcalladapterjson.h"
#include "callreqdata.h"
#include "calljsondef.h"
#include "json/json.h"

#include <android/log.h>

using namespace std;

TEST(ProtocolCallAdapter, ProtocolGetCurrentCallAdapterJson) {
    struct {
        int callState;
        int index;
        int toa;
        int isMpty;
        int isMt;
        int als;
        int callType;
        int isVoicePrivacy;
        char number[MAX_DIAL_NUM + 2] = {0};
        int numPresentation;
        string name; /* This is encoded data so can't be verified in test*/
        int namePresentation;
        int nameDcsType;
        int uusType;
        int uusDcs;
        char uusData[MAX_UUS_DATA_LEN + 2] = {0}; // 1 extra byte to check case where CP sends more data
        int srvccCall;
        int audioQuality;
        char forwardedNum[MAX_SS_NUM_LEN + 2] = {0}; // 1 extra byte to check case where CP sends more data
    } testVector[] = {
        { SIT_CALL_STATE_ACTIVE, 0 /* index */, 0x81 /* UNKNOWN toa */, SIT_CALL_IS_MPTY_SINGLE, SIT_CALL_IS_MT_MO, 0 /* als */,
          SIT_CALL_CALL_TYPE_VOICE, SIT_CALL_IS_VOICE_PRIVACY_INACTIVATED , "number1", SIT_CALL_PRESENTATION_ALLOWED,
          "name1", SIT_CALL_PRESENTATION_ALLOWED, 0x00 /* name dcs */,
          SIT_CALL_UUS_TYPE_TYPE1_IMPLICIT, SIT_CALL_UUS_DCS_USP, "uus_data1",
          0 /* srvcc call */, SIT_CALL_AUDIO_QUALITY_AMR, "forwardedNumber1" },

       { SIT_CALL_STATE_HOLDING, 1 /* index */, 0x91 /* International toa */, SIT_CALL_IS_MPTY_MULTIPARTY, SIT_CALL_IS_MT_MT, 0 /* als */,
          SIT_CALL_CALL_TYPE_VIDEO, SIT_CALL_IS_VOICE_PRIVACY_ACTIVATED, "91number2", SIT_CALL_PRESENTATION_RESTRICTED,
          "name2", SIT_CALL_PRESENTATION_RESTRICTED, 0x00 /* name dcs */,
          SIT_CALL_UUS_TYPE_TYPE1_IMPLICIT, SIT_CALL_UUS_DCS_USP, "uus_data2",
          0 /* srvcc call */, SIT_CALL_AUDIO_QUALITY_AMR, "forwardedNumber2" },

       // Exact same like first test case but with all char[] like value number/uusData/forwardedNum data more than defind limit
        { SIT_CALL_STATE_ACTIVE, 0 /* index */, 0x81 /* UNKNOWN toa */, SIT_CALL_IS_MPTY_SINGLE, SIT_CALL_IS_MT_MO, 0 /* als */,
          SIT_CALL_CALL_TYPE_VOICE, SIT_CALL_IS_VOICE_PRIVACY_INACTIVATED , { [0 ... 81] = '1', [82] = '2', [83] = 0 }, SIT_CALL_PRESENTATION_ALLOWED,
          "name1", SIT_CALL_PRESENTATION_ALLOWED, 0x00 /* name dcs */,
          SIT_CALL_UUS_TYPE_TYPE1_IMPLICIT, SIT_CALL_UUS_DCS_USP, { [0 ... 127] = '1', [128] = '2', [129] = 0 } ,
          0 /* srvcc call */, SIT_CALL_AUDIO_QUALITY_AMR, { [0 ... 31] = '1', [32] = '2', [33] = 0 } },
    };


    struct {
        int callState;
        int index;
        int toa;
        bool isMpty;
        bool isMt;
        bool isVoice;
        bool isVideo;
        bool isVoicePrivacy;
        int numPresent;
        int namePresent;
        char als;
        char number[MAX_DIAL_NUM + 1] = {0};
        string name; /* This can't be compared as it is decoded name*/
        int uusType;
        int uusDcs;
        char uusData[MAX_UUS_DATA_LEN + 1] = {0};
        int srvccCall;
        int audioQuality;
        char forwardedNumber[MAX_SS_NUM_LEN + 1] = {0};
    } rilTestVector[] = {
        { RIL_CALL_ACTIVE, 0 /* index */, 0x81 /* UNKNOWN toa */, false /* isMpty */, false /* isMt */, true /* isVoice */,
          false /* isVideo */, RIL_CALL_CDMA_VOICEPRIVACY_INACTIVE, RIL_CALL_NAME_PRESENTATION_ALLOW, RIL_CALL_NAME_PRESENTATION_ALLOW, 0 /* als */, "number1", "decodedName",
        RIL_UUS_TYPE1_IMPLICIT, RIL_UUS_DCS_USP, "uus_data1", 0 /* srvccCall */, AMR, "forwardedNumber1"},

        { RIL_CALL_HOLDING, 1 /* index */, 0x91 /* International toa */, true /* isMpty */, true /* isMt */, false /* isVoice */,
          true /* isVideo */, RIL_CALL_CDMA_VOICEPRIVACY_ACTIVE, RIL_CALL_NAME_PRESENTATION_RESTRICT, RIL_CALL_NAME_PRESENTATION_RESTRICT, 0 /* als */, {0}, "decodedName2",
        RIL_UUS_TYPE1_IMPLICIT, RIL_UUS_DCS_USP, "uus_data2", 0 /* srvccCall */, AMR, "forwardedNumber2"},

        { RIL_CALL_ACTIVE, 0 /* index */, 0x81 /* UNKNOWN toa */, false /* isMpty */, false /* isMt */, true /* isVoice */,
          false /* isVideo */, RIL_CALL_CDMA_VOICEPRIVACY_INACTIVE, RIL_CALL_NAME_PRESENTATION_ALLOW, RIL_CALL_NAME_PRESENTATION_ALLOW, 0 /* als */,
           { [0 ... 81] = '1', [82] = 0 }, "decodedName",
        RIL_UUS_TYPE1_IMPLICIT, RIL_UUS_DCS_USP, { [0 ... 127] = '1', [128] = 0 }, 0 /* srvccCall */, AMR, { [0 ... 31] = '1', [32] = 0 } },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonCallInfo;
        jsonCallInfo[ns_call_call_info::key_call_state] = testVector[i].callState;
        jsonCallInfo[ns_call_call_info::key_index] = testVector[i].index;
        jsonCallInfo[ns_call_call_info::key_toa] = testVector[i].toa;
        jsonCallInfo[ns_call_call_info::key_is_mpty] = testVector[i].isMpty;
        jsonCallInfo[ns_call_call_info::key_is_mt] = testVector[i].isMt;
        jsonCallInfo[ns_call_call_info::key_als] = testVector[i].als;
        jsonCallInfo[ns_call_call_info::key_call_type] = testVector[i].callType;
        jsonCallInfo[ns_call_call_info::key_is_voice_privacy] = testVector[i].isVoicePrivacy;

        jsonCallInfo[ns_call_call_info::key_number] = string(testVector[i].number, strlen(testVector[i].number));
        jsonCallInfo[ns_call_call_info::key_number_presentation] = testVector[i].numPresentation;
        jsonCallInfo[ns_call_call_info::key_name] = testVector[i].name;
        jsonCallInfo[ns_call_call_info::key_name_presentation] = testVector[i].namePresentation;
        jsonCallInfo[ns_call_call_info::key_name_dcs_type] = testVector[i].nameDcsType;

        jsonCallInfo[ns_call_call_info::key_uus_type] = testVector[i].uusType;
        jsonCallInfo[ns_call_call_info::key_uus_dcs] = testVector[i].uusDcs;
        jsonCallInfo[ns_call_call_info::key_uus_data] = string(testVector[i].uusData, strlen(testVector[i].uusData));

        jsonCallInfo[ns_call_call_info::key_srvcc_call] = testVector[i].srvccCall;
        jsonCallInfo[ns_call_call_info::key_audio_quality] = testVector[i].audioQuality;
        jsonCallInfo[ns_call_call_info::key_forwarded_num] = string(testVector[i].forwardedNum, strlen(testVector[i].forwardedNum));

        Json::Value jsonRspData;
        jsonRspData[ns_call_get_current_calls_rsp::key_current_call_list].append(jsonCallInfo);

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_CURRENT_CALLS, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetCurrentCallAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CURRENT_CALLS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        CallInfo pCallInfo = {};
        int apiStatus = testAdapter.GetCallInfo(&pCallInfo, 0 ); // we are always creating rsp with 1 call so index will be 0
        if( apiStatus < 0 ) {
            printf("[%s] apiStatus = %d\n", __FUNCTION__, apiStatus);
        } else {
            EXPECT_EQ(testAdapter.HasValidLength(), true);
            EXPECT_EQ(testAdapter.GetCallNum(), 1);

            EXPECT_EQ(pCallInfo.m_state, rilTestVector[i].callState);
            EXPECT_EQ(pCallInfo.m_nIndex, rilTestVector[i].index);
            EXPECT_EQ(pCallInfo.m_toa, rilTestVector[i].toa);
            EXPECT_EQ(pCallInfo.m_isMParty, rilTestVector[i].isMpty);
            EXPECT_EQ(pCallInfo.m_isVoice, rilTestVector[i].isVoice);
            EXPECT_EQ(pCallInfo.m_isVideo, rilTestVector[i].isVideo);
            EXPECT_EQ(pCallInfo.m_isVoicePrivacy, rilTestVector[i].isVoicePrivacy);
            EXPECT_EQ(pCallInfo.m_numPresent, rilTestVector[i].numPresent);
            EXPECT_EQ(pCallInfo.m_namePresent, rilTestVector[i].namePresent);
            EXPECT_EQ(pCallInfo.m_als, rilTestVector[i].als);
            EXPECT_EQ(strcmp(pCallInfo.m_number, rilTestVector[i].number), 0);
            //EXPECT_EQ(pCallInfo.m_name, rilTestVector[i].name); // don't check for name as we don't have decoded name
            EXPECT_EQ(strcmp(pCallInfo.m_forwardedNumber, rilTestVector[i].forwardedNumber), 0);

            EXPECT_EQ(pCallInfo.m_uusInfo.m_uusType, rilTestVector[i].uusType);
            EXPECT_EQ(pCallInfo.m_uusInfo.m_uusDcs, rilTestVector[i].uusDcs);
            EXPECT_TRUE(sizeof(pCallInfo.m_uusInfo.m_uusData) == MAX_UUS_DATA_LEN);
            //printf("[%s] uusData = %s\n", __FUNCTION__, pCallInfo.m_uusInfo.m_uusData);
            //printf("[%s] ril_uusDataLen = %zu\n", __FUNCTION__, strlen(rilTestVector[i].uusData));
            EXPECT_EQ(strncmp(pCallInfo.m_uusInfo.m_uusData, rilTestVector[i].uusData, MAX_UUS_DATA_LEN), 0);
        }
        testAdapter.DebugPrintCallInfo(&pCallInfo);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolGetLastCallFailCauseAdapterJson) {
    struct {
        int cpLastCallFailCause;
        int rilLastCallFailCause;
    } testVector[] = {
        { SIT_CALL_LAST_CALL_FAIL_UNOBTAINABLE_NUMBER /*0x0001*/, CALL_FAIL_UNOBTAINABLE_NUMBER },
        { SIT_CALL_LAST_CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE /*0x03E8*/, CALL_FAIL_CDMA_LOCKED_UNTIL_POWER_CYCLE },
        { SIT_CALL_LAST_CALL_FAIL_ERROR_UNSPECIFIED /*0xFFFF*/, CALL_FAIL_ERROR_UNSPECIFIED},
        { 0x0BBB, 0x0BBB }, // Case where RIL is not expecting this value
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_call_get_last_call_fail_cause_rsp::key_last_call_fail_cause] = testVector[i].cpLastCallFailCause;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_LAST_CALL_FAIL_CAUSE, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetLastCallFailCauseAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_LAST_CALL_FAIL_CAUSE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetLastCallFailCause(), testVector[i].rilLastCallFailCause);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);


        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolEmergencyCallListIndAdapterJson) {
    struct {
        char cpMcc[MAX_MCC_LEN+2] = {0};
        char cpMnc[MAX_MNC_LEN+2] = {0};
        int cpServiceCategory;
        char cpEmcNumber[MAX_EMERGENCY_NUMBER_LEN] = {0};
        int cpSource;
    } testVector[] = {
        {"123", "456", SIT_NET_EMERGENCY_CALL_CATEGORY_POLICE, "100", SIT_CALL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING},
        {"123", "456", SIT_NET_EMERGENCY_CALL_CATEGORY_AMBULANCE, "101", SIT_CALL_EMERGENCY_NUMBER_SOURCE_SIM},
        {"123", "456", SIT_NET_EMERGENCY_CALL_CATEGORY_FIRE, "104", SIT_CALL_EMERGENCY_NUMBER_SOURCE_MODEM_CONFIG},
        {"123", "456", SIT_NET_EMERGENCY_CALL_CATEGORY_AUTO, "110", SIT_CALL_EMERGENCY_NUMBER_SOURCE_DEFAULT},
    };

    struct {
        char rilMcc[MAX_MCC_LEN+1] = {0};
        char rilMnc[MAX_MNC_LEN+1] = {0};
        int rilServiceCategory;
        char rilEmcNumber[MAX_EMERGENCY_NUMBER_LEN] = {0};
        int rilSource;
    } rilVector[] = {
        {"123", "456", RIL_EMERGENCY_CATEGORY_POLICE, "100", RIL_EMERGENCY_NUMBER_SOURCE_NETWORK_SIGNALING},
        {"123", "456", RIL_EMERGENCY_CATEGORY_AMBULANCE, "101", RIL_EMERGENCY_NUMBER_SOURCE_SIM},
        {"123", "456", RIL_EMERGENCY_CATEGORY_FIRE_BRIGADE, "104", RIL_EMERGENCY_NUMBER_SOURCE_MODEM_CONFIG},
        {"123", "456", RIL_EMERGENCY_CATEGORY_AIEC, "110", RIL_EMERGENCY_NUMBER_SOURCE_DEFAULT},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    Json::Value jsonIndData;
    jsonIndData[ns_call_emc_call_list_ind::key_mcc] = string(testVector[0].cpMcc, strlen(testVector[0].cpMcc));
    jsonIndData[ns_call_emc_call_list_ind::key_mnc] = string(testVector[0].cpMnc, strlen(testVector[0].cpMnc));

    Json::Value jsonEmcNumInfo;
    for (int j = 0; j < size; j++) {
        jsonEmcNumInfo[ns_call_emc_num_info::key_service_category] = testVector[j].cpServiceCategory;
        jsonEmcNumInfo[ns_call_emc_num_info::key_emc_number] = string(testVector[j].cpEmcNumber);
        jsonEmcNumInfo[ns_call_emc_num_info::key_source] = testVector[j].cpSource;

        jsonIndData[ns_call_emc_call_list_ind::key_emc_nums_info_data].append(jsonEmcNumInfo);
    }

    ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_EMERGENCY_CALL_LIST, jsonIndData);
    ASSERT_NE(modemData, nullptr);

    ProtocolEmergencyCallListIndAdapterJson testAdapter(modemData);
    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMERGENCY_CALL_LIST);
    ASSERT_NE(testAdapter.GetParameter(), nullptr);

    //Payload data checks
    EXPECT_EQ(testAdapter.GetVersion(), 4);
    EXPECT_EQ(strcmp(testAdapter.GetMcc(), rilVector[0].rilMcc), 0);
    EXPECT_EQ(strcmp(testAdapter.GetMnc(), rilVector[0].rilMnc), 0);
    EXPECT_EQ(testAdapter.GetNum(), size);

    //Testing for API GetEccInfo
    EccInfo eccInfo;
    testAdapter.GetEccInfo(&eccInfo);
    for (int j = 0; j < testAdapter.GetNum(); j++) {
        EmergencyNumber *emergencyNumber = eccInfo.GetEmergencyNumber(j);
        EXPECT_EQ(emergencyNumber->GetCategory(), rilVector[j].rilServiceCategory);
        EXPECT_EQ(strcmp(emergencyNumber->GetNumber(), rilVector[j].rilEmcNumber), 0);
        EXPECT_EQ(emergencyNumber->GetSource(), rilVector[j].rilSource);
    }
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolCallAdapter, ProtocolSrvccHoIndAdapterJson) {
    struct {
        int hoType;
        int hoFailCause;
        int hoStatus;
    } testVector[] = {
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_STARTED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_COMPLETED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_REINVITED_REQUESTED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_DISCONNECT_REQUESTED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_FAILED },
        { 1, 0, SIT_CALL_SRVCC_HO_STATUS_START_FAILED },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_call_srvcc_ho_ind::key_srvcc_ho_type] = testVector[i].hoType;
        jsonIndData[ns_call_srvcc_ho_ind::key_srvcc_ho_status] = testVector[i].hoStatus;
        jsonIndData[ns_call_srvcc_ho_ind::key_srvcc_ho_fail_cause] = testVector[i].hoFailCause;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SRVCC_HO, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSrvccHoIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SRVCC_HO);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetSrvccHoType(), testVector[i].hoType);
        EXPECT_EQ(testAdapter.GetSrvccHoStatus(), testVector[i].hoStatus);
        EXPECT_EQ(testAdapter.GetSrvccHoFailCause(), testVector[i].hoFailCause);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolSendCallConfirmRespAdapterJson) {
    struct {
        int cpResult;
        int rilResult;
    } testVector[] = {
        { 0, 0 },
        { 1, 1 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_call_call_confirm_rsp::key_result] = testVector[i].cpResult;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_CALL_CONFIRM, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolSendCallConfirmRespAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_CALL_CONFIRM);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetResult(), testVector[i].rilResult);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolGetPreferredVoicePrivacyModeAdapterJson) {
    struct {
        int cpVPMode;
        int rilVPMode;
    } testVector[] = {
        { 0, 0 }, // Standdard privacy mode (public long code mask)
        { 1, 1 }, // Enhanced privacy mode (private long code mask)
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_call_get_cdma_voice_privacy_mode_rsp::key_vp_mode] = testVector[i].cpVPMode;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_CDMA_VOICE_PRIVACY_MODE, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetPreferredVoicePrivacyModeAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CDMA_VOICE_PRIVACY_MODE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetPreferredVoicePrivacyMode(), testVector[i].rilVPMode);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolCdmaOtaProvisionStatusIndAdapterJson) {
    struct {
        int cpOtaType;
        int cpOtaStatus;
        int rilOtaProvisionStatus;
    } testVector[] = {
        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_OK_SPL_UNLOCKED, CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED },
        { SIT_OTA_TYPE_OTASP,  SIT_OTASP_STATUS_OK_IMSIDWNLD, CDMA_OTA_PROVISION_STATUS_IMSI_DOWNLOADED},

        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_OK_COMMIT, CDMA_OTA_PROVISION_STATUS_COMMITTED},
        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_OK_PROGRAMMING, CDMA_OTA_PROVISION_STATUS_COMMITTED},
        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_SUCCESSFUL, CDMA_OTA_PROVISION_STATUS_COMMITTED},

        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_UNSUCCESSFUL, CDMA_OTA_PROVISION_STATUS_SPL_UNLOCKED },

        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_OK_OTAPAVERIFY, -1 },
        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_PROGRESS, -1 },
        { SIT_OTA_TYPE_OTASP, SIT_OTASP_STATUS_LOCK_CODE_PASSWORD_SET, -1 },

        { SIT_OTA_TYPE_OTAPA, SIT_OTAPA_STATUS_CALL_STOP_MODE, CDMA_OTA_PROVISION_STATUS_OTAPA_STOPPED },
        { SIT_OTA_TYPE_OTAPA, SIT_OTAPA_STATUS_CALL_START_MODE, CDMA_OTA_PROVISION_STATUS_OTAPA_STARTED },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_call_ota_provision_status_ind::key_ota_type] = testVector[i].cpOtaType;
        jsonIndData[ns_call_ota_provision_status_ind::key_ota_status] = testVector[i].cpOtaStatus;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_OTA_PROVISION_STATUS, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaOtaProvisionStatusIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_OTA_PROVISION_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetOtaProvisionStatus(), testVector[i].rilOtaProvisionStatus);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolEmergencySupportRatModeIndAdapterJson) {
    struct {
        int cpAvailableTech;
        int rilAvailableTech;
    } testVector[] = {
        { SIT_SUPPORT_RAT_MODE_3GPP, SUPPORT_RAT_MODE_3GPP },
        { SIT_SUPPORT_RAT_MODE_3GPP2, SUPPORT_RAT_MODE_3GPP2 },
        { SIT_SUPPORT_RAT_MODE_ALL, SUPPORT_RAT_MODE_ALL },
        { -1, SUPPORT_RAT_MODE_3GPP } // when CP sends not recognized available tech
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_call_emc_support_rat_mode_ind::key_available_tech] = testVector[i].cpAvailableTech;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_EMERGENCY_SUPPORT_RAT_MODE, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolEmergencySupportRatModeIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_EMERGENCY_SUPPORT_RAT_MODE);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetSupportRatMode(), testVector[i].rilAvailableTech);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolCdmaInfoListIndAdapterJson_DisplayInfoRecord) {
    struct {
        int cpCdmaInfoName;
        int rilCdmaInfoName;

        char displayChar[MAX_ALPHA_INFO_BUF_LEN + 1 + 1] = {0};
        char rilDisplayChar[MAX_ALPHA_INFO_BUF_LEN + 1] = {0};
    } testVector[] = {
        { SIT_CDMA_DISPLAY_INFO_REC, RIL_CDMA_DISPLAY_INFO_REC, "abcd", "abcd" },
        { SIT_CDMA_EXTENDED_DISPLAY_INFO_REC, RIL_CDMA_EXTENDED_DISPLAY_INFO_REC, {[0 ... 64] = '1', [65] = 0}, {[0 ... 63] = '1', [64] = 0} }, // Where CP has sent data more than defined limits MAX_ALPHA_INFO_BUF_LEN (64)
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value displayInfoRecord;
        displayInfoRecord[ns_call_display_info_record::key_display_char] = string(testVector[i].displayChar, strlen(testVector[i].displayChar));

        Json::Value jsonCdmaInfoRecord;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_cdma_info_name] = testVector[i].cpCdmaInfoName;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_display_info_record] = displayInfoRecord;

        Json::Value jsonIndData;
        jsonIndData[ns_call_cdma_info_rec_ind::key_cdma_info_records].append(jsonCdmaInfoRecord);

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_INFO_REC, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaInfoListIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_INFO_REC);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetNumberOfInfoRecs(), 1); // We only have 1 info record in array

        // Check display info record information
        RIL_CDMA_InformationRecord rilCdmaInfoRecord;
        memset(&rilCdmaInfoRecord, 0x00, sizeof(rilCdmaInfoRecord));
        testAdapter.GetCdmaInfo(rilCdmaInfoRecord, 0); // We only have 1 info record in array so use index as 0
        EXPECT_EQ(rilCdmaInfoRecord.name, testVector[i].rilCdmaInfoName);
        EXPECT_TRUE(sizeof(rilCdmaInfoRecord.rec.display.alpha_buf) == RIL_CDMA_ALPHA_INFO_BUFFER_LENGTH);
        EXPECT_EQ(strncmp(rilCdmaInfoRecord.rec.display.alpha_buf, testVector[i].displayChar, RIL_CDMA_ALPHA_INFO_BUFFER_LENGTH), 0);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolCdmaInfoListIndAdapterJson_NumInfoRecord) {
    struct {
        int cpCdmaInfoName;
        int rilCdmaInfoName;

        char number[MAX_NUMBER_INFO_BUFFER_LEN + 2] = {0}; // 1 extra byte for case when CP sends data more than allowed limit
        int numberType;
        int numberPlan;
        int pi; // 0 = allowed, 1 = Restrict, 2 = Not avilable
        int si;
    } testVector[] = {
        { SIT_CDMA_CALLED_PARTY_NUMBER_INFO_REC, RIL_CDMA_CALLED_PARTY_NUMBER_INFO_REC, "number1", 11, 12, 0, 13},
        { SIT_CDMA_CALLING_PARTY_NUMBER_INFO_REC, RIL_CDMA_CALLING_PARTY_NUMBER_INFO_REC, {[0 ... 81] = '2', [82] = 0}, 21, 22, 1, 23 },
        { SIT_CDMA_CONNECTED_NUMBER_INFO_REC, RIL_CDMA_CONNECTED_NUMBER_INFO_REC, "number3", 31, 32, 2, 33},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value numInfoRecord;
        numInfoRecord[ns_call_num_info_record::key_number] = string(testVector[i].number, strlen(testVector[i].number));
        numInfoRecord[ns_call_num_info_record::key_number_type] = testVector[i].numberType;
        numInfoRecord[ns_call_num_info_record::key_number_plan] = testVector[i].numberPlan;
        numInfoRecord[ns_call_num_info_record::key_presentation_indicator] = testVector[i].pi;
        numInfoRecord[ns_call_num_info_record::key_screening_indicator] = testVector[i].si;

        Json::Value jsonCdmaInfoRecord;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_cdma_info_name] = testVector[i].cpCdmaInfoName;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_num_info_record] = numInfoRecord;

        Json::Value jsonIndData;
        jsonIndData[ns_call_cdma_info_rec_ind::key_cdma_info_records].append(jsonCdmaInfoRecord);

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_INFO_REC, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaInfoListIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_INFO_REC);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetNumberOfInfoRecs(), 1); // We only have 1 info record in array

        // Check num info record information
        RIL_CDMA_InformationRecord rilCdmaInfoRecord;
        memset(&rilCdmaInfoRecord, 0x00, sizeof(rilCdmaInfoRecord));
        testAdapter.GetCdmaInfo(rilCdmaInfoRecord, 0); // We only have 1 info record in array so use index as 0
        EXPECT_EQ(rilCdmaInfoRecord.name, testVector[i].rilCdmaInfoName);
        EXPECT_TRUE(sizeof(rilCdmaInfoRecord.rec.number.buf) == RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH);
        EXPECT_EQ(strncmp(rilCdmaInfoRecord.rec.number.buf, testVector[i].number, RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH), 0);
        EXPECT_EQ(rilCdmaInfoRecord.rec.number.number_type, testVector[i].numberType);
        EXPECT_EQ(rilCdmaInfoRecord.rec.number.number_plan, testVector[i].numberPlan);
        EXPECT_EQ(rilCdmaInfoRecord.rec.number.pi, testVector[i].pi);
        EXPECT_EQ(rilCdmaInfoRecord.rec.number.si, testVector[i].si);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolCdmaInfoListIndAdapterJson_SignalInfoRecord) {
    struct {
        int cpCdmaInfoName;
        int rilCdmaInfoName;

        int isPresent;
        int signalType;
        int alertPitch;
        int signal;
    } testVector[] = {
        { SIT_CDMA_SIGNAL_INFO_REC, RIL_CDMA_SIGNAL_INFO_REC, 1, 2, 3, 4 },
        { SIT_CDMA_SIGNAL_INFO_REC, RIL_CDMA_SIGNAL_INFO_REC, 11, 12, 13, 14 },
        { SIT_CDMA_SIGNAL_INFO_REC, RIL_CDMA_SIGNAL_INFO_REC, 21, 22, 23, 24 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value signalInfoRecord;
        signalInfoRecord[ns_call_signal_info_record::key_is_present] = testVector[i].isPresent;
        signalInfoRecord[ns_call_signal_info_record::key_signal_type] = testVector[i].signalType;
        signalInfoRecord[ns_call_signal_info_record::key_alert_pitch] = testVector[i].alertPitch;
        signalInfoRecord[ns_call_signal_info_record::key_signal] = testVector[i].signal;

        Json::Value jsonCdmaInfoRecord;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_cdma_info_name] = testVector[i].cpCdmaInfoName;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_signal_info_record] = signalInfoRecord;

        Json::Value jsonIndData;
        jsonIndData[ns_call_cdma_info_rec_ind::key_cdma_info_records].append(jsonCdmaInfoRecord);

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_INFO_REC, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaInfoListIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_INFO_REC);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetNumberOfInfoRecs(), 1); // We only have 1 info record in array

        // Check signal info record information
        RIL_CDMA_InformationRecord rilCdmaInfoRecord;
        memset(&rilCdmaInfoRecord, 0x00, sizeof(rilCdmaInfoRecord));
        testAdapter.GetCdmaInfo(rilCdmaInfoRecord, 0); // We only have 1 info record in array so use index as 0
        EXPECT_EQ(rilCdmaInfoRecord.name, testVector[i].rilCdmaInfoName);
        EXPECT_EQ(rilCdmaInfoRecord.rec.signal.isPresent, testVector[i].isPresent);
        EXPECT_EQ(rilCdmaInfoRecord.rec.signal.signalType, testVector[i].signalType);
        EXPECT_EQ(rilCdmaInfoRecord.rec.signal.alertPitch, testVector[i].alertPitch);
        EXPECT_EQ(rilCdmaInfoRecord.rec.signal.signal, testVector[i].signal);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolCdmaInfoListIndAdapterJson_RedirectingInfoRecord) {
    struct {
        int cpCdmaInfoName;
        int rilCdmaInfoName;

        char number[MAX_NUMBER_INFO_BUFFER_LEN + 2] = {0}; // 1 extra byte for case when CP sends data more than allowed limit
        int numberType;
        int numberPlan;
        int pi; // 0 = allowed, 1 = Restrict, 2 = Not avilable
        int si;
        int cpReason;
        int rilReason;
    } testVector[] = {
        { SIT_CDMA_REDIRECTING_NUMBER_INFO_REC, RIL_CDMA_REDIRECTING_NUMBER_INFO_REC, "number1", 11, 12, 0, 13, SIT_REDIRECTING_REASON_CALL_FORWARDING_BUSY, RIL_REDIRECTING_REASON_CALL_FORWARDING_BUSY },
        { SIT_CDMA_REDIRECTING_NUMBER_INFO_REC, RIL_CDMA_REDIRECTING_NUMBER_INFO_REC, {[0 ... 81] = '2', [82] = 0}, 21, 22, 1, 23, SIT_REDIRECTING_REASON_UNKNOWN, RIL_REDIRECTING_REASON_UNKNOWN },
        { SIT_CDMA_REDIRECTING_NUMBER_INFO_REC, RIL_CDMA_REDIRECTING_NUMBER_INFO_REC, "number3", 31, 32, 2, 33, SIT_REDIRECTING_REASON_CALL_FORWARDING_UNCONDITIONAL, RIL_REDIRECTING_REASON_CALL_FORWARDING_UNCONDITIONAL },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value redirInfoRecord;
        redirInfoRecord[ns_call_redirecting_info_record::key_number] = string(testVector[i].number, strlen(testVector[i].number));
        redirInfoRecord[ns_call_redirecting_info_record::key_number_type] = testVector[i].numberType;
        redirInfoRecord[ns_call_redirecting_info_record::key_number_plan] = testVector[i].numberPlan;
        redirInfoRecord[ns_call_redirecting_info_record::key_presentation_indicator] = testVector[i].pi;
        redirInfoRecord[ns_call_redirecting_info_record::key_screening_indicator] = testVector[i].si;
        redirInfoRecord[ns_call_redirecting_info_record::key_reason] = testVector[i].cpReason;

        Json::Value jsonCdmaInfoRecord;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_cdma_info_name] = testVector[i].cpCdmaInfoName;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_redirecting_info_record] = redirInfoRecord;

        Json::Value jsonIndData;
        jsonIndData[ns_call_cdma_info_rec_ind::key_cdma_info_records].append(jsonCdmaInfoRecord);

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_INFO_REC, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaInfoListIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_INFO_REC);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetNumberOfInfoRecs(), 1); // We only have 1 info record in array

        // Check redirecting info record information
        RIL_CDMA_InformationRecord rilCdmaInfoRecord;
        memset(&rilCdmaInfoRecord, 0x00, sizeof(rilCdmaInfoRecord));
        testAdapter.GetCdmaInfo(rilCdmaInfoRecord, 0); // We only have 1 info record in array so use index as 0
        EXPECT_EQ(rilCdmaInfoRecord.name, testVector[i].rilCdmaInfoName);
        EXPECT_TRUE(sizeof(rilCdmaInfoRecord.rec.redir.redirectingNumber.buf) == RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH);
        EXPECT_EQ(strncmp(rilCdmaInfoRecord.rec.redir.redirectingNumber.buf, testVector[i].number, RIL_CDMA_NUMBER_INFO_BUFFER_LENGTH), 0);
        EXPECT_EQ(rilCdmaInfoRecord.rec.redir.redirectingNumber.number_type, testVector[i].numberType);
        EXPECT_EQ(rilCdmaInfoRecord.rec.redir.redirectingNumber.number_plan, testVector[i].numberPlan);
        EXPECT_EQ(rilCdmaInfoRecord.rec.redir.redirectingNumber.pi, testVector[i].pi);
        EXPECT_EQ(rilCdmaInfoRecord.rec.redir.redirectingNumber.si, testVector[i].si);
        EXPECT_EQ(rilCdmaInfoRecord.rec.redir.redirectingReason, testVector[i].rilReason);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapter, ProtocolCdmaInfoListIndAdapterJson_LineCtrInfoRecord) {
    struct {
        int cpCdmaInfoName;
        int rilCdmaInfoName;

        int polarity;
        int toggle;
        int reverse;
        int powerDenial;
    } testVector[] = {
        { SIT_CDMA_LINE_CONTROL_INFO_REC, RIL_CDMA_LINE_CONTROL_INFO_REC, 1, 2, 3, 4 },
        { SIT_CDMA_LINE_CONTROL_INFO_REC, RIL_CDMA_LINE_CONTROL_INFO_REC, 11, 12, 13, 14 },
        { SIT_CDMA_LINE_CONTROL_INFO_REC, RIL_CDMA_LINE_CONTROL_INFO_REC, 21, 22, 23, 24 },
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value lineCtrInfoRecord;
        lineCtrInfoRecord[ns_call_line_ctr_info_record::key_polarity] = testVector[i].polarity;
        lineCtrInfoRecord[ns_call_line_ctr_info_record::key_toggle] = testVector[i].toggle;
        lineCtrInfoRecord[ns_call_line_ctr_info_record::key_reverse] = testVector[i].reverse;
        lineCtrInfoRecord[ns_call_line_ctr_info_record::key_power_denial] = testVector[i].powerDenial;

        Json::Value jsonCdmaInfoRecord;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_cdma_info_name] = testVector[i].cpCdmaInfoName;
        jsonCdmaInfoRecord[ns_call_cdma_info_record::key_line_ctr_info_record] = lineCtrInfoRecord;

        Json::Value jsonIndData;
        jsonIndData[ns_call_cdma_info_rec_ind::key_cdma_info_records].append(jsonCdmaInfoRecord);

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_INFO_REC, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        ProtocolCdmaInfoListIndAdapterJson testAdapter(modemData);
        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_INFO_REC);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        //Payload data checks
        EXPECT_EQ(testAdapter.GetNumberOfInfoRecs(), 1); // We only have 1 info record in array

        // Check signal info record information
        RIL_CDMA_InformationRecord rilCdmaInfoRecord;
        memset(&rilCdmaInfoRecord, 0x00, sizeof(rilCdmaInfoRecord));
        testAdapter.GetCdmaInfo(rilCdmaInfoRecord, 0); // We only have 1 info record in array so use index as 0
        EXPECT_EQ(rilCdmaInfoRecord.name, testVector[i].rilCdmaInfoName);
        EXPECT_EQ(rilCdmaInfoRecord.rec.lineCtrl.lineCtrlPolarityIncluded, testVector[i].polarity);
        EXPECT_EQ(rilCdmaInfoRecord.rec.lineCtrl.lineCtrlToggle, testVector[i].toggle);
        EXPECT_EQ(rilCdmaInfoRecord.rec.lineCtrl.lineCtrlReverse, testVector[i].reverse);
        EXPECT_EQ(rilCdmaInfoRecord.rec.lineCtrl.lineCtrlPowerDenial, testVector[i].powerDenial);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolGetClirAdapterJson) {
    struct {
        int clir_status;
    } testVector[] = {
        { SIT_SS_GET_CLIR_STATUS_NOT_PROVISIONED },
        { SIT_SS_GET_CLIR_STATUS_PROVISIONED },
        { SIT_SS_GET_CLIR_STATUS_UNKNOWN },
        { SIT_SS_GET_CLIR_STATUS_TEMP_RESTRICTED },
        { SIT_SS_GET_CLIR_STATUS_TEMP_ALLOWED },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_ss_get_clir_rsp::key_clir_status] = testVector[i].clir_status;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_CLIR, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetClirAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CLIR);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetClirStatus(), testVector[i].clir_status);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolGetColpAdapterJson) {
    int testVector[] = { 0, 1, 2 };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_ss_get_clir_rsp::key_clir_status] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_COLP, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetColpAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_COLP);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetColpStatus(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolGetCallWaitingAdapterJson) {
    struct {
        int status;
        int ril_status;
        int service_class;
    } testVector[] = {
        { SIT_SS_CALL_FARWARD_STATUS_DISABLE, RIL_SS_MODE_DISABLE, 1 },
        { SIT_SS_CALL_FARWARD_STATUS_ENABLE, RIL_SS_MODE_ENABLE, 2 },
        { SIT_SS_CALL_FARWARD_STATUS_DISABLE, RIL_SS_MODE_DISABLE, 128 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_ss_get_call_waiting_rsp::key_service_status] = testVector[i].status;
        jsonRspData[ns_ss_get_call_waiting_rsp::key_service_class] = testVector[i].service_class;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_CALL_WAITING, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetCallWaitingAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CALL_WAITING);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetServiceStatus(), testVector[i].ril_status);
        EXPECT_EQ(testAdapter.GetServiceClass(), testVector[i].service_class);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolGetClipAdapterJson) {
    struct {
        int clip_status;
        int ril_clip_status;
    } testVector[] = {
        { SIT_SS_GET_CLIP_STATUS_NOT_PROVISIONED, CLIP_NOT_PROVISIONED },
        { SIT_SS_GET_CLIP_STATUS_PROVISIONED, CLIP_PROVISIONED },
        { SIT_SS_GET_CLIP_STATUS_UNKNOWN, CLIP_UNKNOWN },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_ss_get_clip_rsp::key_status] = testVector[i].clip_status;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_CLIP, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetClipAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CLIP);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetClipStatus(), testVector[i].ril_clip_status);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolUssdIndAdapterJson) {
    struct {
        int ussd_status;
        int ril_ussd_status;
        int dcs_type;
        int ussd_len;
        int ussd[MAX_USSD_DATA_LEN];
    } testVector[] = {
        { SIT_SS_USSD_IND_STATUS_USSD_NOTIFY, RIL_USSD_NOTIFY, 15, 5, { 0xaa, 0x98, 0x6c, 0x36, 0x02 } },
        { SIT_SS_USSD_IND_STATUS_USSD_REQUEST, RIL_USSD_REQUEST, 15, 5, { 0xaa, 0x18, 0x2d, 0x36, 0x02 } },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_ss_ussd_ind::key_status] = testVector[i].ussd_status;
        jsonIndData[ns_ss_ussd_ind::key_dcs_type] = testVector[i].dcs_type;
        int ussdLen = testVector[i].ussd_len;
        for(int j = 0; j < ussdLen; j++) {
            jsonIndData[ns_ss_ussd_ind::key_ussd_data].append(testVector[i].ussd[j]);
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_ON_USSD, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolUssdIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_ON_USSD);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        EXPECT_EQ(testAdapter.GetUssdStatus(), testVector[i].ril_ussd_status);
        // Received ussd can be 182, or it can be 182*8/7=208 (gsm7bit case).
        char decodedUssd[MAX_USSD_DATA_LEN + 26];
        int dcs = 0;
        int decodedUssdLen = testAdapter.GetDecodedUssd(decodedUssd, sizeof(decodedUssd), dcs);
        EXPECT_EQ(dcs, testVector[i].dcs_type);
        EXPECT_NE(decodedUssdLen, 0); // not testing decodedUssd as it is decoded value

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolGetColrAdapterJson) {
    int testVector[] = { 0, 1, 2 };

    int size = sizeof(testVector)/sizeof(testVector[0]);
    for(int i = 0; i < size; i++) {
        Json::Value jsonRspData;
        jsonRspData[ns_ss_get_colr_rsp::key_status] = testVector[i];

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_COLR, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetColrAdapterJson testAdapter(modemData);
        //General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_COLR);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);
        //IPC response checks
        EXPECT_EQ(testAdapter.GetColrStatus(), testVector[i]);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolGetCallForwardingStatusAdapterJson) {
    struct {
        int status;
        int ril_status;
        int reason;
        int service_class;
        int toa;
        string number;
        int timeseconds;
    } testVector[] = {
        { SIT_SS_CALL_FARWARD_STATUS_NOT_ACTIVE, RIL_SS_STATUS_NOT_ACTIVE,
            SIT_SS_CALL_FORWARD_READON_MOBILE_BUSY, SIT_SS_SERVICE_CLASS_DATA, 0x81, "8709201234", 30 },
        { SIT_SS_CALL_FARWARD_STATUS_ACTIVE, RIL_SS_STATUS_ACTIVE,
            SIT_SS_CALL_FORWARD_READON_NOT_REACHABLE, SIT_SS_SERVICE_CLASS_VOICE, 0x91,  "867566122779", 20 },
        { SIT_SS_CALL_FARWARD_STATUS_NOT_ACTIVE, RIL_SS_STATUS_NOT_ACTIVE,
            SIT_SS_CALL_FORWARD_READON_NO_REPLY, SIT_SS_SERVICE_CLASS_SMS, 0x91,
            "999991234567890111112345698765123", 25  },
    };

        int size = sizeof(testVector)/sizeof(testVector[0]);
        Json::Value jsonRspData;
        Json::Value jsonCallFrwrdStatus;
        for(int i = 0; i < size; i++) {
            Json::Value jsonCallFrwdInfo;
            jsonCallFrwdInfo[ns_ss_call_forward_status::key_status] = testVector[i].status;
            jsonCallFrwdInfo[ns_ss_call_forward_status::key_reason] = testVector[i].reason;
            jsonCallFrwdInfo[ns_ss_call_forward_status::key_srvc_class] = testVector[i].service_class;
            jsonCallFrwdInfo[ns_ss_call_forward_status::key_toa] = testVector[i].toa;
            jsonCallFrwdInfo[ns_ss_call_forward_status::key_number] = testVector[i].number;
            jsonCallFrwdInfo[ns_ss_call_forward_status::key_time_secs] = testVector[i].timeseconds;
            jsonCallFrwrdStatus.append(jsonCallFrwdInfo);
        }
        jsonRspData[ns_ss_get_call_forward_status_rsp::key_call_frwrd_status_list] = jsonCallFrwrdStatus;

        ModemData *modemData = ModemDataBuilder::BuildJsonRsp(SIT_GET_CALL_FORWARD_STATUS, 0, RCM_E_SUCCESS, jsonRspData);
        ASSERT_NE(modemData, nullptr);

        ProtocolGetCallForwardingStatusAdapterJson testAdapter(modemData);
        // General checks for resposne
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), SIT_GET_CALL_FORWARD_STATUS);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);
        EXPECT_EQ(testAdapter.GetErrorCode(), RIL_E_SUCCESS);

        //IPC checks
        EXPECT_EQ(size, testAdapter.GetCfNum());
        EXPECT_TRUE(testAdapter.HasValidLength());
        EXPECT_EQ(testAdapter.GetCfInfo(NULL, 0), -1);

        RIL_CallForwardInfo cfInfo[size];
        EXPECT_EQ(testAdapter.GetCfInfo(cfInfo, -1), -2);
        EXPECT_EQ(testAdapter.GetCfInfo(cfInfo, 8), -2);
        EXPECT_EQ(testAdapter.GetCfInfo(cfInfo, 3), -2);

        for (int i = 0; i < size; i++) {
            testAdapter.GetCfInfo(&cfInfo[i], i);
        }

        for (int i = 0; i < size; i++) {
            EXPECT_EQ(cfInfo[i].status, testVector[i].ril_status);
            EXPECT_EQ(cfInfo[i].reason, testVector[i].reason);
            EXPECT_EQ(cfInfo[i].serviceClass, testVector[i].service_class);
            EXPECT_EQ(cfInfo[i].toa, testVector[i].toa);
            EXPECT_EQ(cfInfo[i].timeSeconds, testVector[i].timeseconds);
            EXPECT_TRUE(strlen(cfInfo[i].number) <= MAX_SS_NUM_LEN);
            if (i == 1) {
                EXPECT_EQ(strcmp(cfInfo[i].number, "+867566122779"), 0);
            } else if (i == 2) {
                EXPECT_EQ(strcmp(cfInfo[i].number, "+999991234567890111112345698765"), 0);
            } else {
                EXPECT_EQ(strcmp(cfInfo[i].number, testVector[i].number.c_str()), 0);
            }
        }
        // calling once for better coverage
        testAdapter.DebugPrintCfInfo(&cfInfo[0]);

        delete modemData;
        modemData = nullptr;

}

TEST(ProtocolCallAdapterJson, ProtocolSsSvcIndAdapterJson) {
    struct {
        int noti_type;
        int ril_noti_type;
        int code;
        int index;
        unsigned int type;
        string number;
    } testVector[] = {
        // type and number key is not recieved when noti_type is MO
        { SIT_SS_SSNOTI_TYPE_MO, RIL_SSNOTI_TYPE_MO, SIT_SS_CODE1_SOME_COND_CF_ACTIVE, 1, 0, "" },
        { SIT_SS_SSNOTI_TYPE_MO, RIL_SSNOTI_TYPE_MO, SIT_SS_CODE1_CALL_WAITING, 0, 0, "" },
        { SIT_SS_SSNOTI_TYPE_MT, RIL_SSNOTI_TYPE_MT, SIT_SS_CODE2_CUG_CALL, 3, 32, "13128735728" },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_ss_supp_svc_notification_ind::key_noti_type] = testVector[i].noti_type;
        jsonIndData[ns_ss_supp_svc_notification_ind::key_code] = testVector[i].code;
        jsonIndData[ns_ss_supp_svc_notification_ind::key_index] = testVector[i].index;
        if (testVector[i].noti_type == SIT_SS_SSNOTI_TYPE_MT) {
            jsonIndData[ns_ss_supp_svc_notification_ind::key_number_type] = testVector[i].type;
            jsonIndData[ns_ss_supp_svc_notification_ind::key_number] = testVector[i].number;
        }

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_SUPP_SVC_NOTIFICATION, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolSsSvcIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_SUPP_SVC_NOTIFICATION);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        EXPECT_EQ(testAdapter.GetNotificationType(), testVector[i].ril_noti_type);
        EXPECT_EQ(testAdapter.GetCode(), testVector[i].code);
        EXPECT_EQ(testAdapter.GetCugIndex(), testVector[i].index);
        if (testVector[i].noti_type == SIT_SS_SSNOTI_TYPE_MT) {
            string number = testVector[i].number;
            int numberLen = number.length();
            EXPECT_EQ(testAdapter.GetSSType(), testVector[i].type);
            EXPECT_EQ(strlen(testAdapter.GetNumber()), strlen(number.c_str()));
            EXPECT_EQ(strcmp(testAdapter.GetNumber(), number.c_str()), 0);
            EXPECT_EQ(testAdapter.GetNumberLength(), numberLen);
        } else {
            EXPECT_EQ(testAdapter.GetNumberLength(), 0);
        }

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolCallAdapterJson, ProtocolCdmaCallWaitingIndAdapterJson) {
    struct {
        string number;
        int number_presentation;
        string name;
        int signal_type;
        int alert_pitch;
        int signal;
        int number_type;
        int number_plan;
    } testVector[] = {
        { "18618453972", 0, "test", 0, 0, 0, 1, 3 },
        { "18618453972", 1, "abcd", 2, 1, 3, 1, 9 },
        { "9900012345", 0, "qwerty", 3, 2, 5, 2, 0 },
    };

    int size = sizeof(testVector)/sizeof(testVector[0]);

    for (int i = 0; i < size; i++) {
        Json::Value jsonIndData;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_number] = testVector[i].number;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_number_presentation] = testVector[i].number_presentation;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_name] = testVector[i].name;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_signal_type] = testVector[i].signal_type;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_alert_pitch] = testVector[i].alert_pitch;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_signal] = testVector[i].signal;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_number_type] = testVector[i].number_type;
        jsonIndData[ns_ss_cdma_call_waiting_ind::key_number_plan] = testVector[i].number_plan;

        ModemData *modemData = ModemDataBuilder::BuildJsonInd(SIT_IND_CDMA_CALL_WAITING, jsonIndData);
        ASSERT_NE(modemData, nullptr);

        // General IPC checks
        ProtocolCdmaCallWaitingIndAdapterJson testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), SIT_IND_CDMA_CALL_WAITING);
        ASSERT_NE(testAdapter.GetParameter(), nullptr);

        // Payload Check
        RIL_CDMA_CallWaiting_v6 callWaitingNoti;
        testAdapter.GetCwInfo(&callWaitingNoti);

        const char *pNumber = testVector[i].number.c_str();
        const char *pName = testVector[i].name.c_str();
        EXPECT_EQ(callWaitingNoti.numberPresentation, testVector[i].number_presentation);
        if (callWaitingNoti.numberPresentation == 0) {
            EXPECT_EQ(strlen(callWaitingNoti.number), strlen(pNumber));
            EXPECT_EQ(strcmp(callWaitingNoti.number, pNumber), 0);
        } else {
            EXPECT_EQ(callWaitingNoti.number, nullptr);
        }

        EXPECT_EQ(strlen(callWaitingNoti.name), strlen(pName));
        EXPECT_EQ(strcmp(callWaitingNoti.name, pName), 0);
        EXPECT_EQ(callWaitingNoti.signalInfoRecord.isPresent, 1);  // harcoded to SIGNAL_INFO_REC_PRESENT
        EXPECT_EQ(callWaitingNoti.signalInfoRecord.signalType, testVector[i].signal_type);
        EXPECT_EQ(callWaitingNoti.signalInfoRecord.alertPitch, testVector[i].alert_pitch);
        EXPECT_EQ(callWaitingNoti.signalInfoRecord.signal, testVector[i].signal);
        EXPECT_EQ(callWaitingNoti.number_type, testVector[i].number_type);
        EXPECT_EQ(callWaitingNoti.number_plan, testVector[i].number_plan);

        delete modemData;
        modemData = nullptr;
    }
}
