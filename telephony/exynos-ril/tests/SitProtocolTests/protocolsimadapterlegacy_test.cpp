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
#include "legacy/uicc/protocolsimadapterlegacy.h"

#include <android/log.h>

using namespace std;


TEST(ProtocolSimAdapter, ProtocolSimStatusAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE card_state;
        BYTE universal_pin_state;
        BYTE application_num;
        sit_sim_apps_status_info apps_status_info[MAX_SIM_APPS_INFO_COUNT];
        sit_sim_get_sim_status_rsp_ext ext_rsp;
    } testVector = {
        .card_state = RIL_CARDSTATE_PRESENT,
        .universal_pin_state = RIL_PINSTATE_DISABLED,
        .application_num = 1,
        .apps_status_info = {
                {
                    RIL_APPTYPE_USIM,
                    RIL_APPSTATE_READY,
                    RIL_PERSOSUBSTATE_UNKNOWN,
                    3,
                    {'a','i','d'},
                    9,
                    {'a','p','p','_','l','a','b','e','l'},
                    0,
                    RIL_PINSTATE_DISABLED,
                    RIL_PINSTATE_DISABLED,
                    3, 10, 3, 10
                }
        },
        .ext_rsp.esim_no_profile = 0,
        .ext_rsp.physical_slot_id = 0,
        .ext_rsp.atr_length = 3,
        .ext_rsp.atr = {'a','t','r'},
        .ext_rsp.iccid_length = 5,
        .ext_rsp.iccid = {'i','c','c','i','d'},
        .ext_rsp.eid_length = 3,
        .ext_rsp.eid = {'e','i','d'},
        .ext_rsp.port_id = 0
    };
#pragma pack(pop)

    unsigned int sitId = SIT_GET_SIM_STATUS;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimStatusAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    BYTE app_label[MAX_SIM_APP_LABEL_LEN] = {};
    int len = testAdapter.GetApplicationLabel(0, app_label);
    for (int i = 0; i < len; i++) {
        EXPECT_EQ(app_label[i], testVector.apps_status_info[0].app_label[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimVerifyPinAdapterLegacy) {
    static int testVector = 3;

    int nPinIndex = 0;
    for (int i = SIT_VERIFY_SIM_PIN; i <= SIT_CHG_SIM_PIN2; i++) {
        if (i == SIT_VERIFY_SIM_PUK || i == SIT_VERIFY_SIM_PUK2) {
            continue;
        }
        ModemData *modemData = ModemDataBuilder::BuildResponse(i, 0, RCM_E_SUCCESS,
                &testVector, sizeof(testVector));
        ASSERT_NE(modemData, nullptr);

        ProtocolSimVerifyPinAdapterLegacy testAdapter(modemData);

        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), (const unsigned int) i);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
        if (i == SIT_VERIFY_SIM_PIN || i == SIT_CHG_SIM_PIN) {
            nPinIndex = 1;
        } else {
            nPinIndex = 2;
        }
        EXPECT_EQ(testAdapter.GetPinIndex(), nPinIndex);
        EXPECT_EQ(testAdapter.GetRemainCount(), testVector);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapter, ProtocolSimVerifyPukAdapterLegacy) {
    static int testVector = 10;

    int nPinIndex = 0;
    for (int i = SIT_VERIFY_SIM_PUK; i <= SIT_VERIFY_SIM_PUK2; i++) {
        if (i == SIT_VERIFY_SIM_PIN2) {
            continue;
        }
        ModemData *modemData = ModemDataBuilder::BuildResponse(i, 0, RCM_E_SUCCESS,
                &testVector, sizeof(testVector));
        ASSERT_NE(modemData, nullptr);

        ProtocolSimVerifyPukAdapterLegacy testAdapter(modemData);

        //General checks for indication
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
        EXPECT_EQ(testAdapter.GetId(), (const unsigned int) i);
        EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
        if (i == SIT_VERIFY_SIM_PUK) {
            nPinIndex = 1;
        } else {
            nPinIndex = 2;
        }
        EXPECT_EQ(testAdapter.GetPukIndex(), nPinIndex);
        EXPECT_EQ(testAdapter.GetRemainCount(), testVector);

        delete modemData;
        modemData = nullptr;
    }
}

TEST(ProtocolSimAdapter, ProtocolSimIOAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE sw1;
        BYTE sw2;
        INT16 response_len;
        BYTE response[MAX_SIM_IO_DATA_LEN];
    } testVector = {
        .sw1 = 0x90,
        .sw2 = 0x00,
        .response_len = 4,
        .response = {}
    };
#pragma pack(pop)

    testVector.response[0] = 0x01;
    testVector.response[1] = 0x02;
    testVector.response[2] = 0x03;
    testVector.response[3] = 0x04;

    unsigned int sitId = SIT_SIM_IO;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimIOAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetSw1(), testVector.sw1);
    EXPECT_EQ(testAdapter.GetSw2(), testVector.sw2);
    EXPECT_EQ(testAdapter.GetResponseLength(), testVector.response_len);
    BYTE *p = testAdapter.GetResponse();
    for (int i = 0; i < static_cast<int>(testVector.response_len); i++) {
        EXPECT_EQ(p[i], testVector.response[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimGetFacilityLockAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE lock_mode;  // NOT USED by f/w anymore
        BYTE service_class;
    } testVector = {
        .lock_mode = 1,
        .service_class = 2
    };
#pragma pack(pop)

    unsigned int sitId = SIT_GET_FACILITY_LOCK;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimGetFacilityLockAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetServiceClass(), testVector.service_class);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimGetSimAuthAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE auth_type;
        BYTE auth_len;
        BYTE auth[MAX_SIM_AUTH_RSP_LEN];
    } testVector = {
        .auth_type = 1,
        .auth_len = 2,
        .auth = {}
    };
#pragma pack(pop)
    testVector.auth[0] = 1;
    testVector.auth[1] = 2;

    unsigned int sitId = SIT_GET_SIM_AUTH;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimGetSimAuthAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetAuthType(), testVector.auth_type);
    EXPECT_EQ(testAdapter.GetAuthLength(), testVector.auth_len);
    unsigned char *p = testAdapter.GetAuth();
    for (int i = 0; i < testVector.auth_len; i++) {
        EXPECT_EQ(*(p + i), testVector.auth[i]);
    }
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector));
    EXPECT_EQ(testAdapter.GetPayloadLength(), static_cast<int>(sizeof(testVector)));

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimTransmitApduChannelAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE sw1;
        BYTE sw2;
        INT16 response_len;
        BYTE response[5];
    } testVector = {
        .sw1 = 0x90,
        .sw2 = 0x00,
        .response_len = 4,
        .response = {1, 2, 3, 4, 0}
    };
#pragma pack(pop)

    unsigned int sitId = SIT_TRANSMIT_SIM_APDU_CHANNEL;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimTransmitApduChannelAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetSw1(), testVector.sw1);
    EXPECT_EQ(testAdapter.GetSw2(), testVector.sw2);
    EXPECT_EQ(testAdapter.GetApduLength(), testVector.response_len);
    uint8_t *p = testAdapter.GetApdu();
    for (int i = 0; i < testVector.response_len; i++) {
        if (p != nullptr) {
            EXPECT_EQ(*(p + i), testVector.response[i]);
        }
    }
    int nLen = testAdapter.GetParameterLength();
    const char *parameter = testAdapter.GetParameter();
    char *t = (char *)&testVector;
    for (int i = 0; i < nLen; i++) {
        if (parameter != nullptr) {
            EXPECT_EQ(parameter[i], t[i]);
        }
    }
    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimGetGbaAuthAdapterLegacy) {
    static BYTE testVector[] = "auth";

    unsigned int sitId = SIT_GET_GBA_CONTEXT;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimGetGbaAuthAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    BYTE* p = testAdapter.GetGbaAuth();
    for (int i = 0; i < (int)sizeof(testVector); i++) {
        EXPECT_EQ(p[i], testVector[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimReadPbEntryLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE pb_type;
        UINT16 recordId;
        UINT16 data_len;
        char entry_data[MAX_PB_ENTRY_LEN];
    } testVector = {
        .pb_type = 1,
        .recordId = 0,
        .data_len = static_cast<UINT16>(strlen("entry_data")),
        .entry_data = {}
    };
#pragma pack(pop)
    strncpy(testVector.entry_data, "entry_data", strlen("entry_data"));

    unsigned int sitId = SIT_READ_PB_ENTRY;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimReadPbEntryLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetRecordId(), static_cast<int>(testVector.recordId));
    EXPECT_EQ(testAdapter.GetPbType(), static_cast<int>(testVector.pb_type));
    EXPECT_EQ(testAdapter.GetDataLength(), static_cast<int>(testVector.data_len));
    char *p = testAdapter.GetEntryData();
    for (int i = 0; i < testVector.data_len; i++) {
        EXPECT_EQ(p[i], testVector.entry_data[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimPbStorageInfoAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE pb_type;
        UINT16 total_count;
        UINT16 used_count;
    } testVector = {
        .pb_type = 1,
        .total_count = 2,
        .used_count = 3
    };
#pragma pack(pop)

    unsigned int sitId = SIT_GET_PB_STORAGE_INFO;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimPbStorageInfoAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetPbType(), testVector.pb_type);
    EXPECT_EQ(testAdapter.GetTotalCount(), testVector.total_count);
    EXPECT_EQ(testAdapter.GetUsedCount(), testVector.used_count);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimPbCapaAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE entry_num;
        sit_sim_pb_capa pb_list[MAX_PB_ENTRY_NUM];
    } testVector = {
        1, {{1,2,3,4}}
    };
#pragma pack(pop)

    unsigned int sitId = SIT_GET_3G_PB_CAPA;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimPbCapaAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetEntryNum(), testVector.entry_num);
    int entryList[4 * MAX_PB_ENTRY_NUM] = {};
    testAdapter.GetPbCapa(entryList, testVector.entry_num);
    for (int i = 0, j = 0; i < (4 * testVector.entry_num);) {
        if (i > 0) j = i / 4;
        EXPECT_EQ(entryList[i++], testVector.pb_list[j].pb_type);
        EXPECT_EQ(entryList[i++], testVector.pb_list[j].index_max);
        EXPECT_EQ(entryList[i++], testVector.pb_list[j].entry_max);
        EXPECT_EQ(entryList[i++], testVector.pb_list[j].used_count);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimGetCarrierRestrictionsAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        int32_t allowed_carriers_len;
        int32_t excluded_carriers_len;
        SIT_CARRIER_INFO allowed_carriers;
        SIT_CARRIER_INFO excluded_carriers;
        uint8_t allowed_carriers_prioritized;
        uint8_t multisim_policy;
    } testVector = {};
#pragma pack(pop)
    testVector.allowed_carriers_len = 1;
    testVector.excluded_carriers_len = 1;

    testVector.allowed_carriers.mcc[0] = '0';
    testVector.allowed_carriers.mcc[1] = '0';
    testVector.allowed_carriers.mcc[2] = '1';
    testVector.allowed_carriers.mnc[0] = '0';
    testVector.allowed_carriers.mnc[1] = '1';
    testVector.allowed_carriers.match_type = 0;
    testVector.allowed_carriers.match_len = strlen("ALL");
    strncpy((char*)testVector.allowed_carriers.match_data, (const char*)"ALL", strlen("ALL"));

    testVector.excluded_carriers.mcc[0] = '0';
    testVector.excluded_carriers.mcc[1] = '0';
    testVector.excluded_carriers.mcc[2] = '1';
    testVector.excluded_carriers.mnc[0] = '0';
    testVector.excluded_carriers.mnc[1] = '1';
    testVector.excluded_carriers.match_type = 2;
    testVector.excluded_carriers.match_len = strlen("SPN");
    strncpy((char*)testVector.excluded_carriers.match_data, (const char*)"SPN", strlen("SPN"));
    testVector.allowed_carriers_prioritized = true;
    testVector.multisim_policy = 1;

    unsigned int sitId = SIT_GET_CARRIER_RESTRICTIONS;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimGetCarrierRestrictionsAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetAllowedCarrierLength(), testVector.allowed_carriers_len);
    EXPECT_EQ(testAdapter.GetExcludedCarrierLength(), testVector.excluded_carriers_len);
    EXPECT_EQ(testAdapter.GetAllowedCarriersPrioritized(),
            static_cast<bool>(testVector.allowed_carriers_prioritized));
    EXPECT_EQ(testAdapter.GetMultiSimPolicy(), testVector.multisim_policy);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolUiccSubStatusChangeAdapterLegacy) {
    static int32_t testVector = 1;
    unsigned int sitId = SIT_IND_UICC_SUBSCRIPTION_STATE_CHANGED;
    ModemData *modemData =
            ModemDataBuilder::BuildIndication(sitId, &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolUiccSubStatusChangeAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetState(), testVector);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector));
    const int32_t *p = (int32_t *)testAdapter.GetParameter();
    EXPECT_EQ(*p, testVector);

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimLockInfoAdapterLegacy) {
#pragma pack(push, 1)
    static struct testArgs {
        BYTE policy;
        BYTE status;
        BYTE lockType;
        BYTE maxRetryCount;
        BYTE reaminCount;
        UINT16 lockCodeCount;
        char lockCode[SIT_MAX_LOCK_CODE_DATA];
    } testVector = {
        .policy = 1,
        .status = 2,
        .lockType = 3,
        .maxRetryCount = 4,
        .reaminCount = 5,
        .lockCodeCount = 2,
        .lockCode = {}
    };
#pragma pack(pop)

    testVector.lockCode[0] = 0;
    testVector.lockCode[1] = 1;
    testVector.lockCode[2] = 2;
    testVector.lockCode[3] = 3;

    unsigned int sitId = SIT_OEM_GET_SIM_LOCK_INFO;
    ModemData *modemData = ModemDataBuilder::BuildResponse(sitId, 0, RCM_E_SUCCESS,
            &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimLockInfoAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetErrorCode(), RCM_E_SUCCESS);
    EXPECT_EQ(testAdapter.GetPolicy(), testVector.policy);
    EXPECT_EQ(testAdapter.GetStatus(), testVector.status);
    EXPECT_EQ(testAdapter.GetLockType(), testVector.lockType);
    EXPECT_EQ(testAdapter.GetMaxRetryCount(), testVector.maxRetryCount);
    EXPECT_EQ(testAdapter.GetRemainCount(), testVector.reaminCount);
    EXPECT_EQ(testAdapter.GetLockCodeCount(), testVector.lockCodeCount);
    EXPECT_EQ(testAdapter.GetLockCodeSize(), testVector.lockCodeCount * 2);
    for (int i = 0; i < testAdapter.GetLockCodeSize(); i++) {
        EXPECT_EQ(*(testAdapter.GetLockCode() + i), testVector.lockCode[i]);
    }

    delete modemData;
    modemData = nullptr;
}

TEST(ProtocolSimAdapter, ProtocolSimTrayStatusChangeAdapterLegacy) {
    static uint8_t testVector = 2;

    unsigned int sitId = SIT_IND_SIM_TRAY_STATUS_CHANGED;
    ModemData *modemData =
            ModemDataBuilder::BuildIndication(sitId, &testVector, sizeof(testVector));
    ASSERT_NE(modemData, nullptr);

    ProtocolSimTrayStatusChangeAdapterLegacy testAdapter(modemData);

    //General checks for indication
    EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
    EXPECT_EQ(testAdapter.GetId(), sitId);
    EXPECT_EQ(testAdapter.GetState(), (int32_t)testVector);
    EXPECT_EQ(testAdapter.GetParameterLength(), sizeof(testVector));
    const uint8_t *p = (uint8_t *)testAdapter.GetParameter();
    EXPECT_EQ(*p, testVector);

    delete modemData;
    modemData = nullptr;
}
