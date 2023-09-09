/*
 * Copyright Samsung Electronics Co., LTD.
 *
 * This software is proprietary of Samsung Electronics.
 * No part of this software, either material or conceptual may be copied or distributed, transmitted,
 * transcribed, stored in a retrieval system or translated into any human or computer language in any form by any means,
 * electronic, mechanical, manual or otherwise, or disclosed
 * to third parties without the express written permission of Samsung Electronics.
 */
#include <gtest/gtest.h>
#include <string>

#include "rildata.h"
#include "servicestate.h"
#include "simcardstatus.h"

TEST(RilData, RilData_test) {
    char value[] = "test RilData";
    constexpr int testLength = sizeof(value);
    auto testVector = new char[testLength]{};
    memcpy(testVector, value, sizeof(value));

    auto pRilData = std::make_unique<RilData>();
    pRilData->rildata = testVector;
    pRilData->length = testLength;

    EXPECT_TRUE(memcmp(pRilData->GetData(), testVector, sizeof(value))==0);
    EXPECT_EQ(pRilData->GetDataLength(), sizeof(value));
    auto pRilDataClone = std::unique_ptr<RilData>(pRilData->Clone());
    EXPECT_EQ(pRilDataClone.get(), nullptr);
}

TEST(RilData, RilDataString_test) {
    std::string testVector = "test RilDataString";

    auto pRilDataString = std::make_unique<RilDataString>();
    if (pRilDataString.get()) {
        pRilDataString->SetString(testVector.c_str());
        EXPECT_TRUE(strncmp(pRilDataString->GetString(), testVector.c_str(), testVector.length())==0);

        testVector = "test RilDataString2";
        pRilDataString->SetString(testVector.c_str());
        EXPECT_TRUE(strncmp(pRilDataString->GetString(), testVector.c_str(), testVector.length())==0);

        int val = 10;
        testVector = std::to_string(val);
        pRilDataString->SetString(val);
        EXPECT_TRUE(strncmp(pRilDataString->GetString(), testVector.c_str(), testVector.length())==0);

        auto pRilDataStringClone = std::unique_ptr<RilDataString>(pRilDataString->Clone());
        ASSERT_NE(pRilDataStringClone, nullptr);
        EXPECT_TRUE(strncmp(pRilDataString->GetString(), pRilDataStringClone->GetString(), testVector.length())==0);
    }
}

TEST(RilData, RilDataStrings_test) {
    std::string testVector1 = "test RilDataString";
    std::string testVector2 = "test RilDataString2";
    std::string testVector3 = "test RilDataString3";

    auto pRilDataStrings = std::make_unique<RilDataStrings>(3);
    if (pRilDataStrings.get()) {
        pRilDataStrings->SetString(0, testVector1.c_str());
        pRilDataStrings->SetString(1, testVector2.c_str());
        pRilDataStrings->SetString(2, testVector3.c_str());
        EXPECT_FALSE(pRilDataStrings->SetString(10, "out-of-range"));

        EXPECT_TRUE(strncmp(pRilDataStrings->GetString(0), testVector1.c_str(), testVector1.length())==0);
        EXPECT_TRUE(strncmp(pRilDataStrings->GetString(1), testVector2.c_str(), testVector2.length())==0);
        EXPECT_TRUE(strncmp(pRilDataStrings->GetString(2), testVector3.c_str(), testVector3.length())==0);

        std::string tmp = "overwrite";
        pRilDataStrings->SetString(0, tmp.c_str());
        EXPECT_TRUE(strncmp(pRilDataStrings->GetString(0), tmp.c_str(), tmp.length())==0);
    }
}

TEST(RilData, RilDataRaw_test) {
    char testVector[] = "test RilDataRaw";

    auto pRilDataRaw = std::make_unique<RilDataRaw>();
    ASSERT_NE(pRilDataRaw.get(), nullptr);
    pRilDataRaw->SetData(testVector, sizeof(testVector));

    EXPECT_TRUE(memcmp(pRilDataRaw->GetData(), testVector, sizeof(testVector))==0);
    EXPECT_EQ(pRilDataRaw->GetDataLength(), sizeof(testVector));

    char tmp[] = "tmp1";
    pRilDataRaw->SetData(tmp, sizeof(tmp));
    auto pRilDataRawClone = std::unique_ptr<RilDataRaw>(pRilDataRaw->Clone());
    ASSERT_NE(pRilDataRawClone.get(), nullptr);
    EXPECT_TRUE(memcmp(pRilDataRaw->GetData(), pRilDataRawClone->GetData(), pRilDataRaw->GetDataLength())==0);
}

TEST(RilData, RilDataTemplate_test) {
    {
        // return a reference
        ServiceState ss;
        ss.setOperatorName("long", "short");
        ss.setDataRegState(1);
        ss.setVoiceRegState(2);
        auto test = std::make_unique<RilDataTemplate<const ServiceState&>>(
                [&]() -> const ServiceState& { return ss; });
        EXPECT_TRUE(test->Get() == ss);
    }

    {
        // return a value
        ServiceState ss;
        ss.setOperatorName("long", "short");
        ss.setDataRegState(1);
        ss.setVoiceRegState(2);
        auto test = std::make_unique<RilDataTemplate<ServiceState>>(
                [=]() -> ServiceState { return ss; });
        EXPECT_TRUE(test->Get() == ss);
    }

    {
        // return a reference
        SimCardStatus cardStatus = SimCardStatus::create({
                .card_state = RIL_CARDSTATE_PRESENT,
                .physicalSlotId = 1,
                .atr = (char*) "3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
                .iccid = (char*) "8982051506304310894F",
                .eid =(char*) "89033023426200000330025426265873",
            });
        auto test = std::make_unique<RilDataTemplate<const SimCardStatus&>>(
                [&]() -> const SimCardStatus& { return cardStatus; });
        EXPECT_TRUE(test->Get() == cardStatus);
    }

    {
        // return a value
        SimCardStatus cardStatus = SimCardStatus::create({
                .card_state = RIL_CARDSTATE_PRESENT,
                .physicalSlotId = 1,
                .atr = (char*) "3BDE9600803FC7A0FFFFE073FE215B64534304308105F2",
                .iccid = (char*) "8982051506304310894F",
                .eid =(char*) "89033023426200000330025426265873",
            });
        auto test = std::make_unique<RilDataTemplate<SimCardStatus>>(
                [=]() -> SimCardStatus { return cardStatus; });
        EXPECT_TRUE(test->Get() == cardStatus);
    }
}

TEST(RilData, RilDataListHodler_int) {
    auto testVector = { 1, 2, 3, 4, 5, 4, 3, 2, 1 };
    RilDataListHolder<int> test = testVector;
    EXPECT_NE(test.GetData(), nullptr);
    EXPECT_EQ(test.GetDataLength(), (uint32_t)testVector.size() * sizeof(int));
    size_t i = 0;
    for (auto it = std::begin(testVector);
        it != std::end(testVector); ++it) {
        EXPECT_EQ(*((int *)test.GetData() + i++), *it);
    }
}