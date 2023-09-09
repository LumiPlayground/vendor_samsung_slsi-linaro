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
 * simdatabuilder_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */

#include <gtest/gtest.h>
#include "simdatabuilder.h"
#include "constdef.h"
#include <base/rildef.h>
#include <sstream>
#include <librilutils/textutils.h>
#include "testutils.h"

inline std::string toHexString(uint8_t *data, size_t datalen) {
    const char *HEX = "0123456789ABCDEF";
    std::stringstream ss;
    if (data) {
        for (size_t i = 0; i < datalen; i++) {
            ss << HEX[(data[i] >> 4) & 0xF];
            ss << HEX[data[i] & 0xF];
        }
    }
    return ss.str();
}

TEST(SimDataBuilder, BuildSimIoResponse) {

    int sw1 = 0x91;
    int sw2 = 0x00;
    size_t datalen = INTrand(false) % 200 + 1;
    uint8_t data[MAX_SIM_IO_DATA_LEN];
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimIoResponse(sw1, sw2, datalen, data);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SIM_IO_Response));
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_EQ(test->sw1, sw1);
        EXPECT_EQ(test->sw2, sw2);
        EXPECT_TRUE(TextUtils::Equals(test->simResponse, toHexString(data, datalen)));
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildSimIoResponse_0x62_0x80) {
    int sw1 = 0x91;
    int sw2 = 0x00;
    uint8_t data[] = {
            0x62, 0x00,
            0x80, 0x02, 0x0C, 0x0D,
            0x82, 0x04, 0x40, 0x0D, 0x00, 0x0F,
            0x83, 0x02, 0x0A, 0x0B,
            0x81, 0x00,
            0x88, 0x00,
            0x8A, 0x00,
            0x8B, 0x00,
            0x8C, 0x00,
            0xA5, 0x00,
            0xAB, 0x00,
    };
    size_t datalen = sizeof(data);
    data[1] = datalen - 2;

    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimIoResponse(sw1, sw2, datalen, data);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SIM_IO_Response));
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_EQ(test->sw1, sw1);
        EXPECT_EQ(test->sw2, sw2);

        uint8_t testResponse[15] = {};
        testResponse[2] = 0x0C;
        testResponse[3] = 0x0D;
        testResponse[4] = 0x0A;
        testResponse[5] = 0x0B;
        testResponse[6] = 0x04;
        testResponse[9] = 0xFF;
        testResponse[10] = 0xFF;
        testResponse[11] = 0x01;
        testResponse[12] = 0x02;
        testResponse[13] = 0x40;
        testResponse[14] = 0x0F;
        EXPECT_TRUE(TextUtils::Equals(test->simResponse, toHexString(testResponse, 15)));
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildSimIoFcpTemplateResponse) {

    int sw1 = 0x91;
    int sw2 = 0x00;
    size_t datalen = INTrand(false) % 200 + 1;
    uint8_t data[MAX_SIM_IO_DATA_LEN];
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimIoFcpTemplateResponse(sw1, sw2, datalen, data);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SIM_IO_Response));
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_EQ(test->sw1, sw1);
        EXPECT_EQ(test->sw2, sw2);
        EXPECT_TRUE(TextUtils::Equals(test->simResponse, toHexString(data, datalen)));
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildSimGetIsimAuthResponse) {
    size_t size = INTrand(false) % 20 + 1;
    uint8_t testVector[MAX_SIM_IO_DATA_LEN];

    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimGetIsimAuthResponse(0, 0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildSimGetIsimAuthResponse(size, 0);
    EXPECT_EQ(rildata, nullptr);

    rildata = testBuilder.BuildSimGetIsimAuthResponse(size, testVector);
    if (rildata) {
        RilDataString *test = (RilDataString *)rildata;
        EXPECT_NE(test->GetString(), nullptr);
        if (test->GetString()) {
            EXPECT_TRUE(TextUtils::Equals(test->GetString(), toHexString(testVector, size)));
        }
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildSimGetSimAuthResponse) {
    uint8_t sw1 = 0x91;
    uint8_t sw2 = 0x01;
    uint8_t testVector[] = { 0x00, 0x01, 0x02, 0x03, 0x04, sw1, sw2 };
    size_t size = 5;

    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimGetSimAuthResponse(0, 0, 0, 0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildSimGetSimAuthResponse(0, 0, size, 0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildSimGetSimAuthResponse(0, 0, size, testVector);
    EXPECT_EQ(rildata, nullptr);


    rildata = testBuilder.BuildSimGetSimAuthResponse(ISIM_AUTH_GSM, size + 4, size, testVector);
    if (rildata) {
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_EQ(test->sw1, sw1);
        EXPECT_EQ(test->sw2, sw2);
        delete rildata;
    }

    rildata = testBuilder.BuildSimGetSimAuthResponse(ISIM_AUTH_3G, size + 4, size, testVector);
    if (rildata) {
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_EQ(test->sw1, sw1);
        EXPECT_EQ(test->sw2, sw2);
        delete rildata;
    }

    testVector[0] = 5;
    rildata = testBuilder.BuildSimGetSimAuthResponse(ISIM_AUTH_3G, size + 4, size, testVector);
    if (rildata) {
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_EQ(test->sw1, sw1);
        EXPECT_EQ(test->sw2, sw2);
        delete rildata;
    }

}

TEST(SimDataBuilder, BuildSimGetGbaAuthResponse) {
    size_t size = INTrand(false) % 20 + 1;
    uint8_t testVector[MAX_SIM_IO_DATA_LEN];

    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimGetGbaAuthResponse(0, 0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildSimGetGbaAuthResponse(size, 0);
    EXPECT_EQ(rildata, nullptr);

    rildata = testBuilder.BuildSimGetGbaAuthResponse(size, testVector);
    if (rildata) {
        RilDataString *test = (RilDataString *)rildata;
        EXPECT_NE(test->GetString(), nullptr);
        if (test->GetString()) {
            EXPECT_TRUE(TextUtils::Equals(test->GetString(), toHexString(testVector, size)));
        }
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildSimCloseChannelResponse) {
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimCloseChannelResponse();
    EXPECT_EQ(rildata, nullptr);
}

TEST(SimDataBuilder, BuildSimTransmitApduBasicResponse) {
    uint8_t sw1 = 0x91;
    uint8_t sw2 = 0x01;
    uint8_t testVector[] = { 0, 1, 2, 3, 4, 5, 6, 7, sw1, sw2 } ;
    size_t size = sizeof(testVector);

    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimTransmitApduBasicResponse(size, testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SIM_IO_Response));
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_NE(test, nullptr);
        if (test) {
            EXPECT_EQ(test->sw1, sw1);
            EXPECT_EQ(test->sw2, sw2);
            EXPECT_TRUE(TextUtils::Equals(test->simResponse, toHexString(testVector, size - 2)));
        }
        delete rildata;
    }

    // no resp. size must be 2
    testVector[0] = sw1;
    testVector[1] = sw2;
    rildata = testBuilder.BuildSimTransmitApduBasicResponse(2, testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SIM_IO_Response));
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_NE(test, nullptr);
        if (test) {
            EXPECT_EQ(test->sw1, sw1);
            EXPECT_EQ(test->sw2, sw2);
            EXPECT_EQ(test->simResponse, nullptr);
        }
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildSimTransmitApduChannelResponse) {
    uint8_t sw1 = 0x91;
    uint8_t sw2 = 0x01;
    uint8_t testVector[] = { 0, 1, 2, 3, 4, 5, 6, 7 } ;
    size_t size = sizeof(testVector);

    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildSimTransmitApduChannelResponse(sw1, sw2, size, testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SIM_IO_Response));
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_NE(test, nullptr);
        if (test) {
            EXPECT_EQ(test->sw1, sw1);
            EXPECT_EQ(test->sw2, sw2);
            EXPECT_TRUE(TextUtils::Equals(test->simResponse, toHexString(testVector, size)));
        }
        delete rildata;
    }

    // no resp. size must be 2
    rildata = testBuilder.BuildSimTransmitApduChannelResponse(sw1, sw2, 0, 0);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SIM_IO_Response));
        RIL_SIM_IO_Response *test = (RIL_SIM_IO_Response *)rildata->GetData();
        EXPECT_NE(test, nullptr);
        if (test) {
            EXPECT_EQ(test->sw1, sw1);
            EXPECT_EQ(test->sw2, sw2);
            EXPECT_EQ(test->simResponse, nullptr);
        }
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildGetImsiResponse) {
    char testVector[] =  "012345678";
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildGetImsiResponse(0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildGetImsiResponse("");
    EXPECT_EQ(rildata, nullptr);

    rildata = testBuilder.BuildGetImsiResponse("012345678");
    if (rildata) {
        RilDataString *test = (RilDataString *)rildata;
        EXPECT_NE(test->GetString(), nullptr);
        if (test->GetString()) {
            EXPECT_TRUE(TextUtils::Equals(test->GetString(), testVector));
        }
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildGetATRResponse) {
    uint8_t testVector[8];
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildGetATRResponse(0, 0);
    EXPECT_EQ(rildata, nullptr);

    rildata = testBuilder.BuildGetATRResponse((char *)testVector, sizeof(testBuilder));
    if (rildata) {
        RilDataString *test = (RilDataString *)rildata;
        EXPECT_NE(test->GetString(), nullptr);
        if (test->GetString()) {
            EXPECT_TRUE(TextUtils::Equals(test->GetString(), toHexString(testVector, sizeof(testVector))));
        }
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildPbCapaResponse) {
    int testVector[10];
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildPbCapaResponse(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        for (size_t i = 0; i < 10; i++) {
            EXPECT_EQ(((RilDataInts *)rildata)->GetInt(i), testVector[i]);
        }
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildIccidInfoIndicate) {
    uint8_t testVector[14];
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildIccidInfoIndicate(0, 0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildIccidInfoIndicate(sizeof(testVector), 0);
    EXPECT_EQ(rildata, nullptr);

    rildata = testBuilder.BuildIccidInfoIndicate(sizeof(testVector), testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_TRUE(TextUtils::Equals(((RilDataString *)rildata)->GetString(),
                toHexString(testVector, sizeof(testVector))));
        delete rildata;
    }
}

TEST(SimDataBuilder, BuildGetSimLockInfoResponse) {
    SimDataBuilder testBuilder;
    const RilData *rildata = testBuilder.BuildGetSimLockInfoResponse(
            0, 1, LOCK_TYPE_PN, 3, 3, 2, "12345#678901");
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SimLockStatus));
        RIL_SimLockStatus *test = (RIL_SimLockStatus *)rildata->GetData();
        EXPECT_NE(test, nullptr);
        if (test) {
            EXPECT_EQ(test->policy, 0);
            EXPECT_EQ(test->status, 1);
            EXPECT_EQ(test->lockType, LOCK_TYPE_PN);
            EXPECT_EQ(test->maxRetryCount, 3);
            EXPECT_EQ(test->remainCount, 3);
            EXPECT_EQ(test->numOfLockCode, 2);
            EXPECT_NE(test->lockCode, nullptr);
            EXPECT_TRUE(TextUtils::Equals(test->lockCode[0], "12345"));
            EXPECT_TRUE(TextUtils::Equals(test->lockCode[1], "678901"));
        }
        delete rildata;
    }
}
