/*
 * Copyright (c) 2022 Samsung Electronics Co., Ltd All Rights Reserved
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
#include "protocolimsmediabuilder.h"
#include "sitdef.h"
#include "rildef.h"
#include "slsi/ril_oem.h"
#include "tokengen.h"
#include "testutils.h"

#include <stdlib.h>

#define MAX_TEST_PAYLOAD 512

TEST(ProtocolImsMediaBuilder, ProtocolImsMediaBuilder_BuildImsMedia) {
    TokenGen::Init();
    unsigned int testVector[][2] = {
            {RIL_REQUEST_OEM_IMS_TEST_RCM, SIT_IMS_MEDIA_TEST_RCM},
            {RIL_REQUEST_OEM_IMS_MEDIA_INIT, SIT_IMS_MEDIA_INIT},
            {RIL_REQUEST_OEM_IMS_MEDIA_OPEN_SESSION, SIT_IMS_MEDIA_OPEN_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_CLOSE_SESSION, SIT_IMS_MEDIA_CLOSE_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_MODIFY_SESSION, SIT_IMS_MEDIA_MODIFY_SESSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_DTMF, SIT_IMS_MEDIA_SEND_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_START_DTMF, SIT_IMS_MEDIA_START_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_STOP_DTMF, SIT_IMS_MEDIA_STOP_DTMF},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_TEXT, SIT_IMS_MEDIA_SEND_TEXT},
            {RIL_REQUEST_OEM_IMS_MEDIA_SEND_HEADER_EXTENSION, SIT_IMS_MEDIA_SEND_HEADER_EXTENSION},
            {RIL_REQUEST_OEM_IMS_MEDIA_SET_MEDIA_QUALITY_THRESHOLD, SIT_IMS_MEDIA_SET_MEDIA_QUALITY_THRESHOLD},
            {RIL_REQUEST_OEM_IMS_MEDIA_GET_OPERATION_MODE, SIT_IMS_GET_OPERATION_MODE},
            {RIL_REQUEST_OEM_IMS_SIP_MESSAGE, SIT_IMS_SIP_MESSAGE},
            {RIL_REQUEST_OEM_IMS_MEDIA_WIFI_CONFIG, SIT_IMS_WIFI_CONFIG_REQ},
            {RIL_REQUEST_OEM_IMS_MEDIA_QUERY_MAC_CE_INFO, SIT_QUERY_MAC_CE_INFO},

    };

    char payload[MAX_TEST_PAYLOAD];
    ProtocolImsMediaBuilder testBuilder;
    for (auto testElement : testVector) {
        size_t len = INTrand(false) % (MAX_TEST_PAYLOAD + 1);
        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildImsMediaPDU(testElement[0], payload, len));
        EXPECT_TRUE(modemData != nullptr);

        ProtocolReqAdapter testAdapter(modemData.get());
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testElement[1]);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + len);
        EXPECT_EQ(testAdapter.GetParameterLength(), len);
        if (len > 0) {
            EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
            EXPECT_TRUE(memcmp(testAdapter.GetParameter(), payload, len) == 0);
        } else {
            EXPECT_TRUE(testAdapter.GetParameter() == nullptr);
        }
    }
}

TEST(ProtocolImsMediaBuilder, ProtocolImsMediaBuilder_BuildImsMedia_invalid) {
    TokenGen::Init();
    int testVector[] = {
        -1,
        RIL_REQUEST_OEM_SET_ENG_MODE,

        };

    ProtocolImsMediaBuilder testBuilder;
    for (auto testElement : testVector) {
        auto modemData = std::unique_ptr<ModemData>(testBuilder.BuildImsMediaPDU(testElement, nullptr, 0));
        EXPECT_TRUE(modemData == nullptr);
    }
}
