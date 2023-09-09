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
#include "protocolgpsbuilder.h"
#include "slsi/ril_oem.h"
#include "rildef.h"
#include "tokengen.h"
#include "testutils.h"

#include <stdlib.h>

#define MAX_TEST_PAYLOAD 512
#define MAX_RCM_SIZE (64 * 1024)

TEST(ProtocolGpsBuilder, ProtocolGpsBuilder_BuildAgpsPDU) {
    TokenGen::Init();

    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    unsigned int testVector[][2] = {
        {RIL_REQUEST_OEM_GPS_SET_FREQUENCY_AIDING, SIT_SET_GPS_FREQUENCY_AIDING},
        {RIL_REQUEST_OEM_GPS_GET_LPP_SUPL_REQ_ECID_INFO, SIT_GET_LPP_SUPL_REQ_ECID_INFO},
        {RIL_REQUEST_OEM_GPS_SET_RRLP_SUPL_REQ_ECID_INFO, SIT_GET_RRLP_SUPL_REQ_ECID_INFO},
        {RIL_REQUEST_OEM_GPS_MO_LOCATION_REQUEST, SIT_GPS_MO_LOCATION_REQUEST},
        {RIL_REQUEST_OEM_GPS_GET_LPP_REQ_SERVING_CELL_INFO, SIT_GET_LPP_REQ_SERVING_CELL_INFO},
        {RIL_REQUEST_OEM_GPS_SET_SUPL_NI_READY, SIT_SET_GPS_SUPL_NI_READY},
        {RIL_REQUEST_OEM_GPS_GET_GSM_EXT_INFO_MSG, SIT_GET_GSM_EXT_INFO_MSG},
        {RIL_REQUEST_OEM_GPS_CONTROL_PLANE_ENABLE, SIT_GPS_CONTROL_PLANE_ENABLE},
        {RIL_REQUEST_OEM_GPS_GNSS_LPP_PROFILE_SET, SIT_GNSS_LPP_PROFILE_SET},
        {RIL_REQUEST_OEM_GPS_SET_GNSS_BLANKING, SIT_SET_GNSS_BLANKING},
        // CDMA & HEDGE GANSS
        {RIL_REQUEST_OEM_GPS_SET_GPS_LOCK_MODE, SIT_SET_GPS_LOCK_MODE},
        {RIL_REQUEST_OEM_GPS_GET_REFERENCE_LOCATION, SIT_GET_REFERENCE_LOCATION},
        {RIL_REQUEST_OEM_GPS_SET_PSEUDO_RANGE_MEASUREMENTS, SIT_SET_PSEUDO_RANGE_MEASUREMENTS},
        {RIL_REQUEST_OEM_GPS_GET_CDMA_PRECISE_TIME_AIDING_INFO, SIT_GET_CDMA_PRECISE_TIME_AIDING_INFO},
        {RIL_REQUEST_OEM_GPS_CDMA_FREQ_AIDING, SIT_GET_GPS_CDMA_FREQ_AIDING},
    };

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];

    ProtocolGpsBuilder testBuilder;
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        size_t len = INTrand(false) % (MAX_TEST_PAYLOAD + 1);
        ModemData *modemData = testBuilder.BuildAgpsPDU(testVector[i][0], payload, len);
        ASSERT_TRUE(modemData != nullptr);
        EXPECT_TRUE(modemData != nullptr);

        ProtocolReqAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_REQUEST);
        EXPECT_EQ(testAdapter.GetId(), testVector[i][1]);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_HEADER) + len);
        EXPECT_EQ(testAdapter.GetParameterLength(), len);
        if (len > 0) {
            EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
            EXPECT_TRUE(memcmp(testAdapter.GetParameter(), payload, len) == 0);
        } else {
            EXPECT_TRUE(testAdapter.GetParameter() == nullptr);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolGpsBuilder, ProtocolGpsBuilder_BuildAgpsPDU_invalid) {
    TokenGen::Init();

    // wrong ID
    int testVector[] = {
        -1,
        RIL_REQUEST_SETUP_DATA_CALL,
        RIL_REQUEST_OEM_SET_ENG_MODE,
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolGpsBuilder testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildAgpsPDU(testVector[i], nullptr, 0);
        EXPECT_TRUE(modemData == nullptr);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolGpsBuilder, ProtocolGpsBuilder_BuildAgpsIndPDU) {
    TokenGen::Init();

    // index 0 : OEM RIL ID
    // index 1 : SIT message ID
    unsigned int testVector[][2] = {
        {RIL_REQUEST_OEM_GPS_SET_GANSS_MEAS_POS_RSP, SIT_SET_GANSS_MEAS_POS_RSP},
        {RIL_REQUEST_OEM_GPS_MEASURE_POS_RSP, SIT_IND_GPS_MEASURE_POSITION_RSP},
        {RIL_REQUEST_OEM_GPS_RELEASE_GPS, SIT_IND_RELEASE_GPS},
        {RIL_REQUEST_OEM_GPS_MT_LOCATION_REQUEST, SIT_IND_GPS_MT_LOCATION_REQUEST},
        {RIL_REQUEST_OEM_GPS_LPP_PROVIDE_CAPABILITIES, SIT_LPP_PROVIDE_CAPABILITIES_IND},
        {RIL_REQUEST_OEM_GPS_LPP_REQUEST_ASSIST_DATA, SIT_IND_LPP_REQUEST_ASSIST_DATA},
        {RIL_REQUEST_OEM_GPS_LPP_PROVIDE_LOCATION_INFO, SIT_LPP_PROVIDE_LOCATION_INFO_IND},
        {RIL_REQUEST_OEM_GPS_LPP_GPS_ERROR_IND, SIT_LPP_GPS_ERROR_IND},
        {RIL_REQUEST_OEM_GPS_SUPL_LPP_DATA_INFO, SIT_IND_SUPL_LPP_DATA_INFO},
        {RIL_REQUEST_OEM_GPS_SUPL_NI_MESSAGE, SIT_IND_SUPL_NI_MESSAGE},
        {RIL_REQUEST_OEM_GPS_RETRIEVE_LOC_INFO, SIT_GPS_RETRIEVE_LOC_INFO},
        {RIL_REQUEST_OEM_GPS_SET_GNSS_BLANKING_TDL, SIT_SET_GNSS_BLANKING_TDL},
        // CDMA & HEDGE GANSS
        {RIL_REQUEST_OEM_GPS_GANSS_AP_POS_CAP_RSP, SIT_IND_GANSS_AP_POS_CAP_RSP},
    };

    // not to initialize a local buffer to use random packets
    char payload[MAX_TEST_PAYLOAD];

    ProtocolGpsBuilder testBuilder;
    int size = sizeof(testVector) / sizeof(testVector[0]);
    for (int i = 0; i < size; i++) {
        size_t len = INTrand(false) % (MAX_TEST_PAYLOAD + 1);
        ModemData *modemData = testBuilder.BuildAgpsIndPDU(testVector[i][0], payload, len);
        ASSERT_TRUE(modemData != nullptr);
        EXPECT_TRUE(modemData != nullptr);

        ProtocolIndAdapter testAdapter(modemData);
        EXPECT_EQ(testAdapter.GetType(), RCM_TYPE_INDICATION);
        EXPECT_EQ(testAdapter.GetId(), testVector[i][1]);
        EXPECT_EQ(testAdapter.GetLength(), sizeof(RCM_IND_HEADER) + len);
        EXPECT_EQ(testAdapter.GetParameterLength(), len);
        if (len > 0) {
            EXPECT_TRUE(testAdapter.GetParameter() != nullptr);
            EXPECT_TRUE(memcmp(testAdapter.GetParameter(), payload, len) == 0);
        } else {
            EXPECT_TRUE(testAdapter.GetParameter() == nullptr);
        }

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}

TEST(ProtocolGpsBuilder, ProtocolGpsBuilder_BuildAgpsIndPDU_invalid) {
    TokenGen::Init();

    // wrong ID
    int testVector[] = {
        -1,
        RIL_UNSOL_NETWORK_SCAN_RESULT,
        RIL_UNSOL_OEM_DISPLAY_ENG_MODE,
    };

    int size = sizeof(testVector) / sizeof(testVector[0]);
    ProtocolGpsBuilder testBuilder;
    for (int i = 0; i < size; i++) {
        ModemData *modemData = testBuilder.BuildAgpsIndPDU(testVector[i], nullptr, 0);
        EXPECT_TRUE(modemData == nullptr);

        if (modemData) {
            delete modemData;
            modemData = nullptr;
        }
    }  // end for i ~
}
