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
#include "protocoladapter.h"
#include "rildef.h"
#include "sitdef.h"
#include "tokengen.h"
#include "testutils.h"

#define MAX_TEST_PAYLOAD 512

TEST(ProtocolBaseAdapter, BasicTests_Request) {
    TokenGen::Init();

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];
    struct {
        unsigned int id;
        uint32_t token;
        void *data;
        size_t datalen;
    } testVector[] = {
        {SIT_GET_CS_REG_STATE, TokenGen::GetInstacne()->GetNext(), 0, 0},
        {SIT_SET_DATA_PROFILE, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
        {SIT_SET_VOLUME, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
        {SIT_GET_SIM_STATUS, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
        {SIT_GET_DATA_CALL_LIST, TokenGen::GetInstacne()->GetNext(), 0, 0},
        {SIT_SET_VOICE_OPERATION, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
    };

    int testVector1Size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < testVector1Size; i++) {
        ModemData *modemData =
                ModemDataBuilder::BuildRequest(testVector[i].id, testVector[i].token,
                        testVector[i].data, testVector[i].datalen);
        ASSERT_TRUE(modemData != nullptr);
        EXPECT_TRUE(modemData != nullptr);

        ProtocolRespAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].id);
        EXPECT_EQ(testAdapter.GetToken(), testVector[i].token);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + testVector[i].datalen);
        EXPECT_EQ(testAdapter.GetParameterLength(), testVector[i].datalen);
        EXPECT_EQ(testAdapter.GetParameter() != nullptr, testVector[i].data != nullptr);
        bool ret = memcmp(testAdapter.GetParameter(),
                testVector[i].data, testVector[i].datalen) == 0;
        EXPECT_TRUE(ret);
        EXPECT_TRUE(testAdapter.IsRequest());
        EXPECT_FALSE(testAdapter.IsResponse());
        EXPECT_FALSE(testAdapter.IsUnsolicitedResponse());
        EXPECT_FALSE(testAdapter.IsSolicitedResponse());

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolBaseAdapter, BasicTests_Response) {
    TokenGen::Init();

    // not to initialize a local buffer to use random packets
     char payload[MAX_TEST_PAYLOAD];

     struct {
         unsigned int id;
         uint32_t token;
         void *data;
         size_t datalen;
     } testVector[] = {
         {SIT_GET_PS_REG_STATE, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
         {SIT_DIAL, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
         {SIT_SEND_SMS, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
         {SIT_GET_SIM_STATUS, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
         {SIT_SET_PREFERRED_NTW_TYPE, TokenGen::GetInstacne()->GetNext(), 0, 0},
         {SIT_SETUP_DATA_CALL, TokenGen::GetInstacne()->GetNext(), payload, (size_t)(INTrand(false) % (MAX_TEST_PAYLOAD + 1))},
     };

     int size = sizeof(testVector) / sizeof(testVector[0]);
     for (int i = 0; i < size; i++) {
         ModemData *modemData =
                 ModemDataBuilder::BuildResponse(testVector[i].id,
                         testVector[i].token, RCM_E_SUCCESS,
                         testVector[i].data, testVector[i].datalen);
         ASSERT_TRUE(modemData != nullptr);
         EXPECT_TRUE(modemData != nullptr);

         ProtocolRespAdapter testAdapter(modemData);
         EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_RESPONSE);
         EXPECT_EQ(testAdapter.GetId(), testVector[i].id);
         EXPECT_EQ(testAdapter.GetToken(), testVector[i].token);
         EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + testVector[i].datalen);
         EXPECT_EQ(testAdapter.GetParameterLength(), testVector[i].datalen);
         EXPECT_EQ(testAdapter.GetParameter() != nullptr, testVector[i].data != nullptr);
         bool ret = memcmp(testAdapter.GetParameter(),
                             testVector[i].data, testVector[i].datalen) == 0;
         EXPECT_TRUE(ret);
         EXPECT_FALSE(testAdapter.IsRequest());
         EXPECT_TRUE(testAdapter.IsResponse());
         EXPECT_FALSE(testAdapter.IsUnsolicitedResponse());
         EXPECT_TRUE(testAdapter.IsSolicitedResponse());

         if (modemData) {
             delete modemData;
             modemData = nullptr;
         }
     } // end for i ~
}

TEST(ProtocolBaseAdapter, BasicTests_Unsolicited) {
    TokenGen::Init();

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];

    struct {
        unsigned int id;
        void *data;
        size_t datalen;
    } testVector[] = {
        {SIT_IND_CALL_STATE_CHANGED, 0, 0},
        {SIT_IND_EMERGENCY_CALL_LIST, payload, (size_t)(INTrand(false) % MAX_TEST_PAYLOAD + 1)},
        {SIT_IND_NEW_SMS, payload, (size_t)(INTrand(false) % MAX_TEST_PAYLOAD + 1)},
        {SIT_SEND_SMS, payload, (size_t)(INTrand(false) % MAX_TEST_PAYLOAD + 1)},
        {SIT_IND_SIM_STATUS_CHANGED, 0, 0},
        {SIT_IND_NTW_STATE_CHANGED, 0, 0},
        {SIT_IND_DATA_CALL_LIST_CHANGED, payload, (size_t)(INTrand(false) % MAX_TEST_PAYLOAD + 1)},
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        ModemData *modemData =
                ModemDataBuilder::BuildIndication(testVector[i].id,
                        testVector[i].data, testVector[i].datalen);
        ASSERT_TRUE(modemData != nullptr);
        EXPECT_TRUE(modemData != nullptr);

        ProtocolIndAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), testVector[i].id);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_IND_HEADER) + testVector[i].datalen);
        EXPECT_EQ(testAdapter.GetParameterLength(), testVector[i].datalen);
        EXPECT_EQ(testAdapter.GetParameter() != nullptr, testVector[i].data != nullptr);
        bool ret = memcmp(testAdapter.GetParameter(),
                            testVector[i].data, testVector[i].datalen) == 0;
        EXPECT_TRUE(ret);
        EXPECT_FALSE(testAdapter.IsRequest());
        EXPECT_TRUE(testAdapter.IsResponse());
        EXPECT_TRUE(testAdapter.IsUnsolicitedResponse());
        EXPECT_FALSE(testAdapter.IsSolicitedResponse());

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    } // end for i ~
}

TEST(ProtocolBaseAdapter, BasicTests_ErrorCode) {
    struct {
        BYTE rcmError;
        unsigned int radioError;
    } testVector[] = {
        {RCM_E_SUCCESS, RIL_E_SUCCESS},
        {RCM_E_RADIO_NOT_AVAILABLE, RIL_E_RADIO_NOT_AVAILABLE},
        {RCM_E_GENERIC_FAILURE, RIL_E_GENERIC_FAILURE},
        {RCM_E_PASSWORD_INCORRECT, RIL_E_PASSWORD_INCORRECT},
        {RCM_E_SIM_PIN2, RIL_E_SIM_PIN2},
        {RCM_E_SIM_PUK2, RIL_E_SIM_PUK2},
        {RCM_E_REQUEST_NOT_SUPPORTED, RIL_E_REQUEST_NOT_SUPPORTED},
        {RCM_E_UNDEFINED_CMD, RIL_E_REQUEST_NOT_SUPPORTED},
        {RCM_E_CANCELLED, RIL_E_CANCELLED},
        {RCM_E_OP_NOT_ALLOWED_DURING_VOICE_CALL, RIL_E_OP_NOT_ALLOWED_DURING_VOICE_CALL},
        {RCM_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW, RIL_E_OP_NOT_ALLOWED_BEFORE_REG_TO_NW},
        {RCM_E_SMS_SEND_FAIL_RETRY, RIL_E_SMS_SEND_FAIL_RETRY},
        {RCM_E_SIM_ABSENT, RIL_E_SIM_ABSENT},
        {RCM_E_SUBSCRIPTION_NOT_AVAILABLE, RIL_E_SUBSCRIPTION_NOT_AVAILABLE},
        {RCM_E_MODE_NOT_SUPPORTED, RIL_E_MODE_NOT_SUPPORTED},
        {RCM_E_FDN_CHECK_FAILURE, RIL_E_FDN_CHECK_FAILURE},
        {RCM_E_ILLEGAL_SIM_OR_ME, RIL_E_ILLEGAL_SIM_OR_ME},
        {RCM_E_MISSING_RESOURCE, RIL_E_MISSING_RESOURCE},
        {RCM_E_NO_SUCH_ELEMENT, RIL_E_NO_SUCH_ELEMENT},
        {RCM_E_SIM_MEMORY_FULL, RIL_E_SIM_FULL},
        {RCM_E_SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED, RIL_E_SIMULTANEOUS_SMS_AND_CALL_NOT_ALLOWED},
        {RCM_E_ACCESS_BARRED, RIL_E_ACCESS_BARRED},
        {RCM_E_BLOCKED_DUE_TO_CALL, RIL_E_BLOCKED_DUE_TO_CALL},
        {RCM_E_RF_HARDWARE_ISSUE, RIL_E_RF_HARDWARE_ISSUE},
        {RCM_E_NO_RF_CALIBRATION_INFO, RIL_E_NO_RF_CALIBRATION_INFO},
        {RCM_E_MODEM_ERR, RIL_E_MODEM_ERR},
        {RCM_E_INVALID_SIM_STATE, RIL_E_INVALID_SIM_STATE},
        {RCM_E_PB_TOO_LONG_DATA, RIL_E_GENERIC_FAILURE},
        {RCM_E_SIM_PIN_REQ, RIL_E_GENERIC_FAILURE},
        {RCM_E_SIM_PUK_REQ, RIL_E_GENERIC_FAILURE},
        {RCM_E_OP_NOT_ALLOWED_DURING_PLMN_SEARCH, RIL_E_GENERIC_FAILURE},
        {RCM_E_NOT_ALLOWED_DURING_PLMN_SEARCH, RIL_E_GENERIC_FAILURE},
        // {-1, RIL_E_GENERIC_FAILURE},     // Impossbile TC, wrong usecase will be blocked with -Werror
        // {1000, RIL_E_GENERIC_FAILURE},   // Impossbile TC, wrong usecase will be blocked with -Werror
    };
    size_t size = sizeof(testVector) / sizeof(testVector[0]);
    for (size_t i = 0; i < size; i++) {
        ModemData *modemData = ModemDataBuilder::BuildResponse(SIT_SET_PREFERRED_DATA_MODEM,
                        i, testVector[i].rcmError);
        ASSERT_TRUE(modemData != nullptr);
        ProtocolRespAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetToken(), i);
        EXPECT_EQ(testAdapter.GetErrorCode(), testVector[i].radioError);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }
}
