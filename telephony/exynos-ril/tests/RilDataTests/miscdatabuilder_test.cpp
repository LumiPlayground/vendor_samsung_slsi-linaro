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
 * miscdatabuilder_test.cpp
 *
 *  Created on: 2021. 8. 9.
 *      Author: sungwoo48.choi
 */
#include <gtest/gtest.h>
#include "miscdatabuilder.h"
#include <librilutils/textutils.h>

TEST(MiscDataBuilder, BuildBaseBandVersionResponse) {
    MiscDataBuilder testBuilder;
    char testVector[] = "SHANNON_QB123456";
    const RilData *rildata = testBuilder.BuildBaseBandVersionResponse(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_TRUE(TextUtils::Equals((char *)rildata->GetData(), testVector));
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildGetTtyModeResponse) {
    MiscDataBuilder testBuilder;
    int testVector = 3;
    const RilData *rildata = testBuilder.BuildGetTtyModeResponse(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(*(int *)rildata->GetData(), testVector);
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildIMEIResponse) {
    MiscDataBuilder testBuilder;
    char testVector[] = "350612345676";
    const RilData *rildata = testBuilder.BuildIMEIResponse(0, 0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildIMEIResponse(0, (uint8_t *)testVector);
    EXPECT_EQ(rildata, nullptr);

    rildata = testBuilder.BuildIMEIResponse(strlen(testVector), (uint8_t *)testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_TRUE(TextUtils::Equals((char *)rildata->GetData(), testVector));
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildIMEISVResponse) {
    MiscDataBuilder testBuilder;
    char testVector[] = "01";
    const RilData *rildata = testBuilder.BuildIMEISVResponse(0, 0);
    EXPECT_EQ(rildata, nullptr);
    rildata = testBuilder.BuildIMEISVResponse(0, (uint8_t *)testVector);
    EXPECT_EQ(rildata, nullptr);

    rildata = testBuilder.BuildIMEISVResponse(strlen(testVector), (uint8_t *)testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_TRUE(TextUtils::Equals((char *)rildata->GetData(), testVector));
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildDevIDResponse) {
    struct DeviceId {
        const char *imei;
        const char *imeiSv;
        const char *meid;
        const char *esn;
    };

    DeviceId testVector[] = {
        {"350612345676",
         "01",
         "0011223344",
         "AABBCCDDEE"},
        {"350612345676350612345676350612345676350612345676",
         "01350612345676350612345676350612345676350612345676",
         "0011223344350612345676350612345676350612345676350612345676",
         "AABBCCDDEE350612345676350612345676350612345676350612345676"},
    };

    auto compareDeviceIdString = [](const char *drivedString, const char *orgString) -> bool {
        if (orgString == nullptr || drivedString == nullptr)
            return false;

        const unsigned int maxDrivedStrLen = 32;
        if (strlen(orgString) > maxDrivedStrLen) {
            return (strlen(drivedString) == maxDrivedStrLen && strncmp(orgString, drivedString, maxDrivedStrLen) == 0);
        } else {
            return (strncmp(orgString, drivedString, maxDrivedStrLen) == 0);
        }
    };

    unsigned int numOfDevIDStrings = sizeof(DeviceId) / sizeof(const char *);
    MiscDataBuilder testBuilder;
    for (auto testDeviceId : testVector) {
        const RilData *rildata = testBuilder.BuildDevIDResponse(
            strlen(testDeviceId.imei), (uint8_t *)testDeviceId.imei,
            strlen(testDeviceId.imeiSv), (uint8_t *)testDeviceId.imeiSv,
            strlen(testDeviceId.meid), (uint8_t *)testDeviceId.meid,
            strlen(testDeviceId.esn), (uint8_t *)testDeviceId.esn);
        EXPECT_NE(rildata, nullptr);
        if (rildata) {
            RilDataStrings *test = (RilDataStrings *)rildata;
            EXPECT_EQ(test->GetDataLength(), sizeof(char *) * numOfDevIDStrings);
            EXPECT_TRUE(compareDeviceIdString(test->GetString(0), testDeviceId.imei));
            EXPECT_TRUE(compareDeviceIdString(test->GetString(1), testDeviceId.imeiSv));
            EXPECT_TRUE(compareDeviceIdString(test->GetString(2), testDeviceId.esn));
            EXPECT_TRUE(compareDeviceIdString(test->GetString(3), testDeviceId.meid));
            delete rildata;
        }
    }
}

TEST(MiscDataBuilder, BuildDisplayEngIndication) {
    MiscDataBuilder testBuilder;
    char testVector[] = { 0, 1, 2, 4, 5, 4, 3, 2, 1, 0 };
    const RilData *rildata = testBuilder.BuildDisplayEngIndication(testVector, sizeof(testVector));
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(testVector));
        EXPECT_TRUE(memcmp(rildata->GetData(), testVector, rildata->GetDataLength()) == 0);
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildLceDataIndication) {
    MiscDataBuilder testBuilder;
    EXPECT_EQ(testBuilder.BuildLceDataIndication(0, 0, 0, 0), nullptr);
}

TEST(MiscDataBuilder, BuildCdmaSubscriptionSource) {
    MiscDataBuilder testBuilder;
    int testVector = 3;
    const RilData *rildata = testBuilder.BuildCdmaSubscriptionSource(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(*(int *)rildata->GetData(), testVector);
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildCdmaSubscription) {
    MiscDataBuilder testBuilder;
    const char *testVector[] = { "MDN", "10", "20", "MIN", "30" };
    const RilData *rildata = testBuilder.BuildCdmaSubscription(
            (char *)testVector[0], std::stoi(testVector[1]),
            std::stoi(testVector[2]), (char *)testVector[3], (uint32_t)std::stoi(testVector[4]));
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        RilDataStrings *test = (RilDataStrings *)rildata;
        EXPECT_EQ(test->GetDataLength(), sizeof(char *) * 5);
        for (size_t i = 0; i < 5; i++) {
            EXPECT_TRUE(TextUtils::Equals(test->GetString(i), testVector[i]));
        }
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildHardwareConfigNV) {
    MiscDataBuilder testBuilder;
    RIL_HardwareConfig testVector = {
        RIL_HardwareConfig_Type::RIL_HARDWARE_CONFIG_MODEM,
        "UUID_123",
        RIL_HardwareConfig_State::RIL_HARDWARE_CONFIG_STATE_ENABLED,
        .cfg.modem = { 0, 0x1FFF, 1, 2, 2 },
    };
    const RilData *rildata = testBuilder.BuildHardwareConfigNV(
            testVector.uuid, testVector.state,
            testVector.cfg.modem.rilModel, testVector.cfg.modem.rat,
            testVector.cfg.modem.maxVoice, testVector.cfg.modem.maxData,
            testVector.cfg.modem.maxStandby);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_HardwareConfig));
        EXPECT_TRUE(memcmp(rildata->GetData(), &testVector, sizeof(RIL_HardwareConfig)) == 0);
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildHardwareConfigRuim) {
    MiscDataBuilder testBuilder;
    RIL_HardwareConfig testVector = {
        RIL_HardwareConfig_Type::RIL_HARDWARE_CONFIG_SIM,
        "UUID_123",
        RIL_HardwareConfig_State::RIL_HARDWARE_CONFIG_STATE_ENABLED,
        .cfg.sim = { "modemUuid_1234" },
    };
    const RilData *rildata = testBuilder.BuildHardwareConfigRuim(
            testVector.uuid, testVector.state, testVector.cfg.sim.modemUuid);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_HardwareConfig));
        EXPECT_TRUE(memcmp(rildata->GetData(), &testVector, sizeof(RIL_HardwareConfig)) == 0);
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildRssiScanResult) {
    MiscDataBuilder testBuilder;
    int total = 100;
    int current = 0;
    int startFrequency = 10;
    int endFrequency = 20;
    int step = 2;
    int16_t result[] = { 1, 2, 3, 4, 5 };
    int resultSize = sizeof(result) / sizeof(result[0]);
    const RilData *rildata = testBuilder.BuildRssiScanResult(
            total, current, startFrequency, endFrequency, step, result, resultSize);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(result) + sizeof(int) * 5);
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildATCommand) {
    MiscDataBuilder testBuilder;
    char testVector[] = "AT+TEST?\r\n";
    const RilData *rildata = testBuilder.BuildATCommand(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), strlen(testVector));
        EXPECT_TRUE(memcmp(rildata->GetData(), testVector, strlen(testVector)) == 0);
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildGetModemStatus) {
    MiscDataBuilder testBuilder;
    int testVector = 3;
    const RilData *rildata = testBuilder.BuildGetModemStatus(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(*(int *)rildata->GetData(), testVector);
        delete rildata;
    }
}

TEST(MiscDataBuilder, SignalStrengthBuilder) {
    RIL_SignalStrength_V1_6 testVector = {};
    SignalStrengthBuilder testBuilder;
    const RilData *rildata = testBuilder.Build(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SignalStrength_V1_4));
        delete rildata;
    }

    testBuilder = SignalStrengthBuilder(HAL_VERSION_CODE(1, 6));
    rildata = testBuilder.Build(testVector);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_SignalStrength_V1_6));
        delete rildata;
    }
}

TEST(MiscDataBuilder, BuildModemInfo) {
    MiscDataBuilder testBuilder;
    char testVector[10];
    const RilData *rildata = testBuilder.BuildModemInfo(1, testVector, sizeof(testVector));
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(testVector) + sizeof(int) * 2);
        EXPECT_EQ(((int *)rildata->GetData())[0], 1);
        EXPECT_EQ(((unsigned /* NEED To CHECK */ int *)rildata->GetData())[1], sizeof(testVector));
        EXPECT_TRUE(memcmp(&((int *)rildata->GetData())[2], testVector, sizeof(testVector)) == 0);
        delete rildata;
    }
}

TEST(MiscDataBuilder, CurrentLinkCapacityEstimateBuilder) {
    RIL_LinkCapacityEstimate_V1_6 testVector = {
        10, 20, 30, 40
    };
    CurrentLinkCapacityEstimateBuilder testBuilder;
    const RilData *rildata = testBuilder.Build(testVector.downlinkCapacityKbps, testVector.uplinkCapacityKbps,
            testVector.DCNRSecondaryDownlinkCapacityKbps, testVector.DCNRSecondaryUplinkCapacityKbps);
    EXPECT_NE(rildata, nullptr);
    if (rildata) {
        EXPECT_EQ(rildata->GetDataLength(), sizeof(RIL_LinkCapacityEstimate_V1_6));
        EXPECT_TRUE(memcmp(rildata->GetData(), &testVector, sizeof(RIL_LinkCapacityEstimate_V1_6)) == 0);
        delete rildata;
    }
}
